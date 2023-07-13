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
#include "absl/types/span.h"
#include "ocpdiag/core/hwinterface/backends/host/host_backend.h"
#include "ocpdiag/core/hwinterface/backends/lib/mock_host_adapter.h"
#include "ocpdiag/core/hwinterface/memory.pb.h"
#include "ocpdiag/core/hwinterface/service.pb.h"
#include "ocpdiag/core/testing/file_utils.h"
#include "ocpdiag/core/testing/parse_text_proto.h"
#include "ocpdiag/core/testing/proto_matchers.h"
#include "ocpdiag/core/testing/status_matchers.h"

namespace ocpdiag::hwinterface::internal {
namespace {

using ::ocpdiag::testing::EqualsProto;
using ::ocpdiag::testing::IsOkAndHolds;
using ::ocpdiag::testing::ParseTextProtoOrDie;
using ::ocpdiag::testing::StatusIs;
using ::testing::Return;

constexpr absl::string_view kTestdataPath =
    "ocpdiag/core/hwinterface/backends/host/testdata";
constexpr absl::string_view kDmiFilePath = "fake_dmi/fake_DMI.hex";
constexpr absl::string_view kExpectedFilePath =
    "get_memory_info/expected.textpb";

// Returns the contents in `filename` under
// "ocpdiag/core/hwinterface/backends/host/testdata"
std::string GetTestDataContents(absl::string_view filename) {
  std::filesystem::path testdata_path(kTestdataPath);
  return testutils::GetDataDependencyFileContents(
      (testdata_path.append(filename)).string());
}

TEST(HostBackend, GetMemoryInfoSucceeds) {
  auto mock_host = std::make_unique<MockHostAdapter>();
  // The human-readable version of the hex can be found at
  // get_memory_info/fake_DMI.txt
  EXPECT_CALL(*mock_host, Read)
      .WillOnce(Return(GetTestDataContents(kDmiFilePath)));

  HostBackend backend(EntityConfiguration{}, std::move(mock_host));
  GetMemoryInfoResponse expected =
      ParseTextProtoOrDie(GetTestDataContents(kExpectedFilePath));

  EXPECT_THAT(backend.GetMemoryInfo(GetMemoryInfoRequest()),
              IsOkAndHolds(EqualsProto(expected)));
}

TEST(HostBackend, GetMemoryInfoPropagateHostAdapterFailure) {
  auto mock_host = std::make_unique<MockHostAdapter>();
  EXPECT_CALL(*mock_host, Read)
      .WillOnce(Return(absl::InternalError("internal error")));

  HostBackend backend(EntityConfiguration{}, std::move(mock_host));

  EXPECT_THAT(backend.GetMemoryInfo(GetMemoryInfoRequest()),
              StatusIs(absl::StatusCode::kInternal, "internal error"));
}

TEST(HostBackend, GetMemoryInfoShouldReturnSize) {
  auto mock_host = std::make_unique<MockHostAdapter>();
  EXPECT_CALL(*mock_host, Read)
      .WillOnce(Return(GetTestDataContents(kDmiFilePath)));

  HostBackend backend(EntityConfiguration{}, std::move(mock_host));
  GetMemoryInfoResponse expected = ParseTextProtoOrDie(
      R"pb(
        dimm_infos {
          key: 0
          value { size_bytes: 17179869184 }
        }
        dimm_infos {
          key: 1
          value { size_bytes: 17179869184 }
        }
      )pb");
  GetMemoryInfoRequest req;
  req.add_info_types(memory::SIZE);

  EXPECT_THAT(backend.GetMemoryInfo(req), IsOkAndHolds(EqualsProto(expected)));
}

TEST(HostBackend, GetMemoryInfoShouldReturnDimmIdentifier) {
  auto mock_host = std::make_unique<MockHostAdapter>();
  EXPECT_CALL(*mock_host, Read)
      .WillOnce(Return(GetTestDataContents(kDmiFilePath)));

  HostBackend backend(EntityConfiguration{}, std::move(mock_host));
  GetMemoryInfoResponse expected = ParseTextProtoOrDie(
      R"pb(
        dimm_infos {
          key: 0
          value {
            identifier { name: "DIMM0" type: "dimm" id: "%%DIMM0%%dimm" }
          }
        }
        dimm_infos {
          key: 1
          value {
            identifier { name: "DIMM1" type: "dimm" id: "%%DIMM1%%dimm" }
          }
        }
      )pb");
  GetMemoryInfoRequest req;
  req.add_info_types(memory::DIMM_IDENTIFIER);

  EXPECT_THAT(backend.GetMemoryInfo(req), IsOkAndHolds(EqualsProto(expected)));
}

TEST(HostBackend, GetMemoryInfoShouldReturnDimmLocation) {
  auto mock_host = std::make_unique<MockHostAdapter>();
  EXPECT_CALL(*mock_host, Read)
      .WillOnce(Return(GetTestDataContents(kDmiFilePath)));

  HostBackend backend(EntityConfiguration{}, std::move(mock_host));
  GetMemoryInfoResponse expected = ParseTextProtoOrDie(
      R"pb(
        dimm_infos {
          key: 0
          value { location { slot_name: "DIMM0" } }
        }
        dimm_infos {
          key: 1
          value { location { slot_name: "DIMM1" } }
        }
      )pb");
  GetMemoryInfoRequest req;
  req.add_info_types(memory::DIMM_LOCATION);

  EXPECT_THAT(backend.GetMemoryInfo(req), IsOkAndHolds(EqualsProto(expected)));
}

TEST(HostBackend, GetMemoryInfoShouldReturnFwMemoryType) {
  auto mock_host = std::make_unique<MockHostAdapter>();
  EXPECT_CALL(*mock_host, Read)
      .WillOnce(Return(GetTestDataContents(kDmiFilePath)));

  HostBackend backend(EntityConfiguration{}, std::move(mock_host));
  GetMemoryInfoResponse expected = ParseTextProtoOrDie(
      R"pb(
        dimm_infos {
          key: 0
          value { fw_memory_type: FW_MEMORYTYPE_DDR4 }
        }
        dimm_infos {
          key: 1
          value { fw_memory_type: FW_MEMORYTYPE_DDR4 }
        }
      )pb");
  GetMemoryInfoRequest req;
  req.add_info_types(memory::FW_MEMORY_TYPE);

  EXPECT_THAT(backend.GetMemoryInfo(req), IsOkAndHolds(EqualsProto(expected)));
}

TEST(HostBackend, GetMemoryInfoShouldReturnSpeed) {
  auto mock_host = std::make_unique<MockHostAdapter>();
  EXPECT_CALL(*mock_host, Read)
      .WillOnce(Return(GetTestDataContents(kDmiFilePath)));

  HostBackend backend(EntityConfiguration{}, std::move(mock_host));
  GetMemoryInfoResponse expected = ParseTextProtoOrDie(
      R"pb(
        dimm_infos {
          key: 0
          value { speed { firmware_speed_mhz: 2666 operating_speed_mhz: 2666 } }
        }
        dimm_infos {
          key: 1
          value { speed { firmware_speed_mhz: 2666 operating_speed_mhz: 2666 } }
        }
      )pb");
  GetMemoryInfoRequest req;
  req.add_info_types(memory::SPEED);

  EXPECT_THAT(backend.GetMemoryInfo(req), IsOkAndHolds(EqualsProto(expected)));
}

TEST(HostBackend, GetMemoryInfoShouldReturnJedec) {
  auto mock_host = std::make_unique<MockHostAdapter>();
  EXPECT_CALL(*mock_host, Read)
      .WillOnce(Return(GetTestDataContents(kDmiFilePath)));

  HostBackend backend(EntityConfiguration{}, std::move(mock_host));
  GetMemoryInfoResponse expected = ParseTextProtoOrDie(
      R"pb(
        dimm_infos {
          key: 0
          value { jedec { manufacturer_index: 1 } }
        }
        dimm_infos {
          key: 1
          value { jedec { manufacturer_index: 2 } }
        }
      )pb");
  GetMemoryInfoRequest req;
  req.add_info_types(memory::JEDEC);

  EXPECT_THAT(backend.GetMemoryInfo(req), IsOkAndHolds(EqualsProto(expected)));
}

}  // namespace
}  // namespace ocpdiag::hwinterface::internal
