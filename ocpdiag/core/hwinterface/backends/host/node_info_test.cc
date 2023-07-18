// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <cstdio>
#include <memory>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "ocpdiag/core/hwinterface/backends/host/host_backend.h"
#include "ocpdiag/core/hwinterface/backends/lib/mock_host_adapter.h"
#include "ocpdiag/core/hwinterface/service.pb.h"
#include "ocpdiag/core/testing/proto_matchers.h"
#include "ocpdiag/core/testing/status_matchers.h"

namespace ocpdiag::hwinterface::internal {
namespace {

using ::ocpdiag::testing::EqualsProto;
using ::ocpdiag::testing::IsOkAndHolds;
using ::ocpdiag::testing::StatusIs;
using ::testing::HasSubstr;
using ::testing::Return;

TEST(HostBackend, GetNodeInfoSucceeds) {
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
              }
            })json"}));

  HostBackend backend(EntityConfiguration{}, std::move(mock_host));

  EXPECT_THAT(backend.GetNodeInfo(GetNodeInfoRequest()),
              IsOkAndHolds(EqualsProto(R"pb(
                info {
                  frus {
                    identifier {
                      name: "ModelName#SerialName"
                      type: "scsi"
                      id: "%%ModelName#SerialName%%scsi"
                    }
                  }
                }
              )pb")));
}

TEST(HostBackend, GetNodeInfoShouldPropagateGetStorageInfoFailure) {
  auto mock_host = std::make_unique<MockHostAdapter>();

  // GetStorageInfo propagates RunCommand failure.
  EXPECT_CALL(*mock_host, RunCommand)
      .WillOnce(Return(HostAdapter::CommandResult{.exit_code = 1,
                                                  .stderr = "smartctl error"}));

  HostBackend backend(EntityConfiguration{}, std::move(mock_host));

  EXPECT_THAT(backend.GetNodeInfo(GetNodeInfoRequest()),
              StatusIs(absl::StatusCode::kInternal,
                       HasSubstr("Exit code: 1. Stderr: smartctl error")));
}

}  // namespace
}  // namespace ocpdiag::hwinterface::internal
