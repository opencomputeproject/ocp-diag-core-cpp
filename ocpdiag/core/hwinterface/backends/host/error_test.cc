// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/backends/host/error.h"

#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/time/time.h"
#include "ocpdiag/core/testing/file_utils.h"
#include "ocpdiag/core/testing/status_matchers.h"

namespace ocpdiag::hwinterface::internal {
namespace {

using ::ocpdiag::testing::IsOk;
using ::ocpdiag::testing::StatusIs;
using ::ocpdiag::testutils::GetDataDependencyFileContents;
using ::testing::ContainerEq;
using ::testing::IsEmpty;

TEST(RasDaemonErrors, ParseErrorReport) {
  RasDaemonErrors ras;

  std::vector<Error> expected{
      {.timestamp =
           absl::FromDateTime(2021, 6, 3, 15, 13, 37, absl::UTCTimeZone()),
       .count = 1,
       .is_correctable = true,
       .is_fatal = false,
       .label = "CPU_SrcID#0_MC#0_Chan#0_DIMM#0",
       .debuginfo = "1 2021-06-03 15:13:37 +0000 1 Corrected "
                    "error(s): FAKE ERROR at "
                    "CPU_SrcID#0_MC#0_Chan#0_DIMM#0 location: "
                    "0:0:0:0, addr 0, grain 5, "
                    "syndrome 0 for EDAC testing only"},
      {.timestamp =
           absl::FromDateTime(2021, 6, 3, 15, 13, 38, absl::UTCTimeZone()),
       .count = 1,
       .is_correctable = true,
       .is_fatal = false,
       .label = "CPU_SrcID#0_MC#0_Chan#0_DIMM#0",
       .debuginfo = "2 2021-06-03 15:13:38 +0000 1 Corrected "
                    "error(s): FAKE ERROR at "
                    "CPU_SrcID#0_MC#0_Chan#0_DIMM#0 location: "
                    "0:0:0:0, addr 0, grain 5, "
                    "syndrome 0 for EDAC testing only"},
      {
          .timestamp =
              absl::FromDateTime(2021, 6, 3, 15, 13, 38, absl::UTCTimeZone()),
          .count = 2,
          .is_correctable = false,
          .is_fatal = false,
          .label = "unknown memory",
          .debuginfo = "3 2021-06-03 15:13:38 +0000 2 Uncorrected error(s): "
                       "FAKE ERROR at "
                       "unknown memory location: 0:0:1:0, addr 0, grain 0, "
                       "syndrome 0 for "
                       "EDAC testing only",
      },
      {
          .timestamp =
              absl::FromDateTime(2021, 6, 3, 15, 13, 39, absl::UTCTimeZone()),
          .count = 1,
          .is_correctable = false,
          .is_fatal = true,
          .label = "CPU_SrcID#0_MC#0_Chan#1_DIMM#0",
          .debuginfo =
              "4 2021-06-03 15:13:39 +0000 1 Fatal error(s): FAKE ERROR at "
              "CPU_SrcID#0_MC#0_Chan#1_DIMM#0 location: 0:1:0:0, addr 0, "
              "grain 5, "
              "syndrome 0 for EDAC testing only",
      },
      {.timestamp =
           absl::FromDateTime(2021, 6, 3, 15, 13, 40, absl::UTCTimeZone()),
       .count = 1,
       .is_correctable = true,
       .is_fatal = false,
       .label = "CPU_SrcID#0_MC#1_Chan#1_DIMM#0",
       .debuginfo =
           "6 2021-06-03 15:13:40 +0000 1 Corrected error(s): FAKE ERROR at "
           "CPU_SrcID#0_MC#1_Chan#1_DIMM#0 location: 1:1:0:0, addr 0, grain 5, "
           "syndrome 0 for EDAC testing only"},
  };

  EXPECT_THAT(ras.ParseErrorReport(GetDataDependencyFileContents(
                  "ocpdiag/core/hwinterface/backends/host/"
                  "testdata/get_errors/error.report")),
              IsOk());
  EXPECT_THAT(ras.memory_errors(), ContainerEq(expected));
}

TEST(RasDaemonErrors, ParseErrorReportNoError) {
  RasDaemonErrors ras;
  EXPECT_THAT(ras.ParseErrorReport("No Memory errors.\n\n"), IsOk());
  EXPECT_THAT(ras.memory_errors(), IsEmpty());
}

TEST(RasDaemonErrors, ParseErrorReportMalformed) {
  RasDaemonErrors ras;
  EXPECT_THAT(ras.ParseErrorReport("Malformed"),
              StatusIs(absl::StatusCode::kInternal,
                       "`ras-mc-ctl --errors`: parsed failed, step: MC_TITLE, "
                       "msg:[malform], line [Malformed]"));
}

TEST(RasDaemonErrors, ParseErrorReportEmpty) {
  RasDaemonErrors ras;
  EXPECT_THAT(ras.ParseErrorReport(""),
              StatusIs(absl::StatusCode::kInternal,
                       "`ras-mc-ctl --errors`: parsed failed, step: MC_TITLE, "
                       "msg:[malform], line []"));
}

}  // namespace
}  // namespace ocpdiag::hwinterface::internal
