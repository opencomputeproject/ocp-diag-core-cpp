// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <memory>
#include <string>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "ocpdiag/core/hwinterface/backends/host/host_backend.h"
#include "ocpdiag/core/hwinterface/backends/lib/mock_host_adapter.h"
#include "ocpdiag/core/hwinterface/service.pb.h"
#include "ocpdiag/core/testing/file_utils.h"
#include "ocpdiag/core/testing/parse_text_proto.h"
#include "ocpdiag/core/testing/proto_matchers.h"
#include "ocpdiag/core/testing/status_matchers.h"

namespace ocpdiag::hwinterface::internal {

using ::ocpdiag::testing::EqualsProto;
using ::ocpdiag::testing::IsOkAndHolds;
using ::ocpdiag::testing::StatusIs;
using ::testing::Return;

constexpr absl::string_view kTestdataPath =
    "ocpdiag/core/hwinterface/backends/host/testdata";
constexpr absl::string_view kDmiFilePath = "fake_dmi/fake_DMI_table_20.hex";

// Returns the contents in `filename` under
// "ocpdiag/core/hwinterface/backends/host/testdata"
std::string GetTestDataContents(absl::string_view filename) {
  std::filesystem::path testdata_path(kTestdataPath);
  return testutils::GetDataDependencyFileContents(
      (testdata_path.append(filename)).string());
}

TEST(HostBackend, MemoryConvertSucceeds) {
  auto mock_host = std::make_unique<MockHostAdapter>();
  EXPECT_CALL(*mock_host, Read)
      .WillRepeatedly(Return(GetTestDataContents(kDmiFilePath)));

  HostBackend backend(EntityConfiguration{}, std::move(mock_host));

  MemoryConvertRequest req;
  req.set_physical_address(1);
  EXPECT_THAT(backend.MemoryConvert(req),
              IsOkAndHolds(EqualsProto(R"pb(identifiers {
                                              name: "DIMM1"
                                              type: "dimm"
                                              id: "%%DIMM1%%dimm"
                                            })pb")));
}

TEST(HostBackend, MemoryConvertInvalidPhysicalAddress) {
  auto mock_host = std::make_unique<MockHostAdapter>();
  EXPECT_CALL(*mock_host, Read)
      .WillRepeatedly(Return(GetTestDataContents(kDmiFilePath)));

  HostBackend backend(EntityConfiguration{}, std::move(mock_host));

  MemoryConvertRequest req;
  req.set_physical_address(0xFFFFFFFFFFF);
  EXPECT_THAT(backend.MemoryConvert(req),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(HostBackend, MemoryConvertUnableToFindMemoryDevice) {
  auto mock_host = std::make_unique<MockHostAdapter>();
  EXPECT_CALL(*mock_host, Read)
      .WillRepeatedly(Return(GetTestDataContents(kDmiFilePath)));

  HostBackend backend(EntityConfiguration{}, std::move(mock_host));

  MemoryConvertRequest req;
  req.set_physical_address(0x00100000001);
  EXPECT_THAT(backend.MemoryConvert(req),
              StatusIs(absl::StatusCode::kNotFound));
}

}  // namespace ocpdiag::hwinterface::internal
