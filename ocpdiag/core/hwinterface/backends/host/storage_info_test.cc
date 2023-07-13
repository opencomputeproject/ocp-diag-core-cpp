// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "ocpdiag/core/hwinterface/backends/host/host_backend.h"
#include "ocpdiag/core/hwinterface/backends/host/host_test_utils.h"
#include "ocpdiag/core/hwinterface/backends/lib/mock_host_adapter.h"
#include "ocpdiag/core/hwinterface/service.pb.h"
#include "ocpdiag/core/hwinterface/storage.pb.h"
#include "ocpdiag/core/testing/parse_text_proto.h"
#include "ocpdiag/core/testing/proto_matchers.h"
#include "ocpdiag/core/testing/status_matchers.h"

namespace ocpdiag::hwinterface::internal {
namespace {

using ::ocpdiag::hwinterface::host::testing::GetTestDataContentsOrDie;
using ::ocpdiag::testing::EqualsProto;
using ::ocpdiag::testing::IsOkAndHolds;
using ::ocpdiag::testing::ParseTextProtoOrDie;
using ::ocpdiag::testing::StatusIs;
using ::testing::HasSubstr;
using ::testing::Return;
using ::testing::SizeIs;
using ::testing::TestParamInfo;
using ::testing::TestWithParam;
using ::testing::UnorderedElementsAreArray;
using ::testing::ValuesIn;

TEST(HostBackend, GetStorageInfoSucceeds) {
  auto mock_host = std::make_unique<MockHostAdapter>();

  EXPECT_CALL(*mock_host, RunCommand)
      .WillOnce(
          Return(HostAdapter::CommandResult{.exit_code = 0, .stdout = R"json(
            {
              "devices": [
                {
                  "name": "/dev/sda",
                  "info_name": "/dev/sda",
                  "type": "scsi",
                  "protocol": "SCSI"
                }
              ]
            })json"}))
      .WillOnce(
          Return(HostAdapter::CommandResult{.exit_code = 0, .stdout = R"json(
            {
              "device": {
                "name": "/dev/sda",
                "info_name": "/dev/sda [SAT]",
                "type": "sat",
                "protocol": "ATA"
              },
              "model_name": "ModelName",
              "serial_number": "SerialName",
              "firmware_version": "T1103BST",
              "user_capacity": {
                "blocks": 209715200,
                "bytes": 107374182400
              },
              "logical_block_size": 512,
              "physical_block_size": 512,
              "rotation_rate": 0,
              "sata_version": {
                "string": "SATA 3.2",
                "value": 255
              },
              "smart_support": {
                "available": true,
                "enabled": true
              },
              "ata_smart_data": {
                "capabilities": {
                  "values": [
                    113,
                    3
                  ],
                  "gp_logging_supported": true
                }
              },
              "ata_sct_capabilities": {
                "value": 49,
                "error_recovery_control_supported": false,
                "feature_control_supported": true,
                "data_table_supported": true
              }
            })json"}))
      .WillOnce(
          Return(HostAdapter::CommandResult{.exit_code = 0, .stdout = R"json(
            {
              "ata_security": {
                "state": 291,
                "string": "ENABLED, PW level MAX, not locked, not frozen [SEC5]",
                "enabled": true,
                "frozen": false,
                "pw_level_max": true,
                "locked": false
              }
            })json"}));

  HostBackend backend(EntityConfiguration{}, std::move(mock_host));

  GetStorageInfoResponse expected = ParseTextProtoOrDie(
      GetTestDataContentsOrDie("get_storage_info/expected.textpb"));

  EXPECT_THAT(backend.GetStorageInfo(GetStorageInfoRequest()),
              IsOkAndHolds(EqualsProto(expected)));
}

struct GetDeviceTypeTestCase {
  std::string test_name;
  std::string version_json;
  int rotation_rate;
  storage::DeviceType device_type;
};

using GetDeviceTypeTest = TestWithParam<GetDeviceTypeTestCase>;

TEST_P(GetDeviceTypeTest, CheckDeviceType) {
  const GetDeviceTypeTestCase& test_case = GetParam();
  auto mock_host = std::make_unique<MockHostAdapter>();
  EXPECT_CALL(*mock_host, RunCommand)
      .WillOnce(
          Return(HostAdapter::CommandResult{.exit_code = 0, .stdout = R"json(
            {
              "devices": [
                {
                  "name": "/dev/sda",
                  "info_name": "/dev/sda",
                  "type": "scsi",
                  "protocol": "SCSI"
                }
              ]
            })json"}))
      .WillOnce(Return(HostAdapter::CommandResult{
          .exit_code = 0,
          .stdout = absl::StrFormat(R"json(
            {
              "device": {
                "name": "/dev/sda",
                "info_name": "/dev/sda [SAT]",
                "type": "sat",
                "protocol": "ATA"
              },
              "model_name": "ModelName",
              "serial_number": "SerialName",
              "firmware_version": "FirmwareVersion",
              "user_capacity": {
                "blocks": 209715200,
                "bytes": 107374182400
              },
              "logical_block_size": 512,
              "physical_block_size": 512,
              "rotation_rate": %d,
              "smart_support": {
                "available": true,
                "enabled": true
              },
              "ata_smart_data": {
                "capabilities": {
                  "values": [
                    113,
                    3
                  ],
                  "gp_logging_supported": true
                }
              },
              "ata_sct_capabilities": {
                "value": 49,
                "error_recovery_control_supported": false,
                "feature_control_supported": true,
                "data_table_supported": true
              },
              %s
            })json",
                                    test_case.rotation_rate,
                                    test_case.version_json)}));
  HostBackend backend(EntityConfiguration{}, std::move(mock_host));
  GetStorageInfoRequest req;
  req.add_info_types(storage::InfoType::DEVICE_TYPE);

  absl::StatusOr<GetStorageInfoResponse> resp = backend.GetStorageInfo(req);

  EXPECT_OK(resp);
  ASSERT_THAT(resp.value().info(), SizeIs(1));
  EXPECT_EQ(resp.value().info()[0].device_type(), test_case.device_type);
}

INSTANTIATE_TEST_SUITE_P(
    GetStorageInfo, GetDeviceTypeTest,
    ValuesIn<GetDeviceTypeTestCase>({
        {.test_name = "IsSsdType",
         .version_json = R"json(
            "sata_version": {
            })json",
         .rotation_rate = 0,
         .device_type = storage::DeviceType::DEVICETYPE_SSD},
        {.test_name = "IsHddType",
         .version_json = R"json(
            "sata_version": {
            })json",
         .rotation_rate = 7200,
         .device_type = storage::DeviceType::DEVICETYPE_HDD},
        {.test_name = "IsNvmeSsdType",
         .version_json = R"json(
            "nvme_version": {
            })json",
         .rotation_rate = 0,
         .device_type = storage::DeviceType::DEVICETYPE_NVME_SSD},
        {.test_name = "IsUnknownType",
         .version_json = R"json(
            "no_version": {
            })json",
         .rotation_rate = 0,
         .device_type = storage::DeviceType::DEVICETYPE_UNKNOWN},
    }),
    [](const TestParamInfo<GetDeviceTypeTest::ParamType>& info) {
      return info.param.test_name;
    });

TEST(HostBackend, GetStorageInfoShouldPropagateCmdFailure) {
  auto mock_host = std::make_unique<MockHostAdapter>();

  EXPECT_CALL(*mock_host, RunCommand)
      .WillOnce(Return(HostAdapter::CommandResult{.exit_code = 1,
                                                  .stderr = "Some failure"}));

  HostBackend backend(EntityConfiguration{}, std::move(mock_host));

  EXPECT_THAT(backend.GetStorageInfo(GetStorageInfoRequest()),
              StatusIs(absl::StatusCode::kInternal,
                       HasSubstr("Exit code: 1. Stderr: Some failure")));
}

TEST(HostBackend, GetStorageInfoShouldPropagateJsonParsingFailure) {
  auto mock_host = std::make_unique<MockHostAdapter>();

  EXPECT_CALL(*mock_host, RunCommand)
      .WillOnce(
          Return(HostAdapter::CommandResult{.exit_code = 0, .stdout = R"json(
            {
              "devices": [
                {
                  "name": "/dev/sda",
                  "info_name": "/dev/sda",
                  "type": "scsi",
                  "protocol": "SCSI"
                }
              ]
              make_an_invalid_json
            })json"}));

  HostBackend backend(EntityConfiguration{}, std::move(mock_host));

  EXPECT_THAT(backend.GetStorageInfo(GetStorageInfoRequest()),
              StatusIs(absl::StatusCode::kInternal,
                       HasSubstr("Unable to parse text to json")));
}

struct CapabilityMappingTestCase {
  std::string test_name;
  std::string ata_smart_data_capabilities_json;
  std::string ata_sct_capabilities_json;
  std::vector<storage::Capability> expected_supported_capabilities;
  std::vector<storage::Capability> expected_enabled_capabilities;
};

using CapabilityMappingTest = TestWithParam<CapabilityMappingTestCase>;

TEST_P(CapabilityMappingTest, TestCapabilityMapping) {
  const CapabilityMappingTestCase& test_case = GetParam();
  auto mock_host = std::make_unique<MockHostAdapter>();
  EXPECT_CALL(*mock_host, RunCommand)
      .WillOnce(
          Return(HostAdapter::CommandResult{.exit_code = 0, .stdout = R"json(
            {
              "devices": [
                {
                  "name": "/dev/sda",
                  "info_name": "/dev/sda",
                  "type": "scsi",
                  "protocol": "SCSI"
                }
              ]
            })json"}))
      .WillOnce(Return(HostAdapter::CommandResult{
          .exit_code = 0,
          .stdout = absl::StrFormat(R"json(
            {
              "device": {
                "name": "/dev/sda",
                "info_name": "/dev/sda [SAT]",
                "type": "sat",
                "protocol": "ATA"
              },
              "model_name": "ModelName",
              "serial_number": "SerialName",
              "firmware_version": "FirmwareVersion",
              "user_capacity": {
                "blocks": 209715200,
                "bytes": 107374182400
              },
              "logical_block_size": 512,
              "physical_block_size": 512,
              "rotation_rate": 0,
              "sata_version": {
                "string": "SATA 3.2",
                "value": 255
              },
              "smart_support": {
                "available": true,
                "enabled": true
              },
              "ata_smart_data": {
                "capabilities": {
                  "values": [
                    113,
                    3
                  ],
                  %s
                }
              },
              "ata_sct_capabilities": {
                "value": 49,
                %s
              }
            })json",
                                    test_case.ata_smart_data_capabilities_json,
                                    test_case.ata_sct_capabilities_json)}));
  HostBackend backend(EntityConfiguration{}, std::move(mock_host));
  GetStorageInfoRequest req;
  req.add_info_types(storage::InfoType::ATA_INFO);

  absl::StatusOr<GetStorageInfoResponse> resp = backend.GetStorageInfo(req);

  EXPECT_OK(resp);
  ASSERT_THAT(resp.value().info(), SizeIs(1));
  std::cout << resp->DebugString() << std::endl;
  EXPECT_THAT(
      resp.value().info()[0].ata_info().supported_capabilities(),
      UnorderedElementsAreArray(test_case.expected_supported_capabilities));
  EXPECT_THAT(
      resp.value().info()[0].ata_info().enabled_capabilities(),
      UnorderedElementsAreArray(test_case.expected_enabled_capabilities));
}

INSTANTIATE_TEST_SUITE_P(
    GetStorageInfo, CapabilityMappingTest,
    ValuesIn<CapabilityMappingTestCase>(
        {{.test_name = "Test1",
          .ata_smart_data_capabilities_json = R"json(
              "gp_logging_supported": true
            )json",
          .ata_sct_capabilities_json = R"json(
              "error_recovery_control_supported": true,
              "feature_control_supported": true,
              "data_table_supported": true
            )json",
          .expected_supported_capabilities =
              {storage::CAP_SMART, storage::CAP_GPL, storage::CAP_SCT,
               storage::CAP_SCT_TAB, storage::CAP_SCT_ERC,
               storage::CAP_SCT_FEAT},
          .expected_enabled_capabilities = {storage::CAP_SMART}},
         {.test_name = "Test2",
          .ata_smart_data_capabilities_json = R"json(
              "gp_logging_supported": false
            )json",
          .ata_sct_capabilities_json = R"json(
              "error_recovery_control_supported": false,
              "feature_control_supported": false,
              "data_table_supported": false
            )json",
          .expected_supported_capabilities = {storage::CAP_SMART,
                                              storage::CAP_SCT},
          .expected_enabled_capabilities = {storage::CAP_SMART}}}),
    [](const TestParamInfo<CapabilityMappingTest::ParamType>& info) {
      return info.param.test_name;
    });

TEST(HostBackend, GetStorageInfoShouldReturnUnknownSecurityMode) {
  auto mock_host = std::make_unique<MockHostAdapter>();
  EXPECT_CALL(*mock_host, RunCommand)
      .WillOnce(
          Return(HostAdapter::CommandResult{.exit_code = 0, .stdout = R"json(
            {
              "devices": [
                {
                  "name": "/dev/sda",
                  "info_name": "/dev/sda",
                  "type": "scsi",
                  "protocol": "SCSI"
                }
              ]
            })json"}))
      .WillOnce(
          Return(HostAdapter::CommandResult{.exit_code = 0, .stdout = R"json(
            {
              "ata_security": {
                "enabled": false
              }
            })json"}));
  HostBackend backend(EntityConfiguration{}, std::move(mock_host));
  GetStorageInfoRequest req;
  req.add_info_types(storage::InfoType::SECURITY_INFO);

  EXPECT_THAT(backend.GetStorageInfo(req), IsOkAndHolds(EqualsProto(R"pb(
                info {
                  security_info {
                    level: SECURITYLEVEL_UNKNOWN
                    mode: SECURITYMODE_UNKNOWN
                  }
                }
              )pb")));
}

}  // namespace
}  // namespace ocpdiag::hwinterface::internal
