// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/client/fake_node_client.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "ocpdiag/core/hwinterface/backends/lib/host_adapter.h"
#include "ocpdiag/core/hwinterface/config.pb.h"
#include "ocpdiag/core/hwinterface/service.pb.h"
#include "ocpdiag/core/testing/parse_text_proto.h"
#include "ocpdiag/core/testing/status_matchers.h"

namespace ocpdiag::hwinterface {
namespace {

using ::ocpdiag::testing::ParseTextProtoOrDie;

// This test checks one handler in detail.
TEST(FakeBackendTest, NominalBehavior) {
  EntityConfiguration config = ParseTextProtoOrDie(R"pb(
    entity { host_address: "abc" }
  )pb");
  FakeNodeClient fake_backend(config);

  // Check the name and host address.
  EXPECT_EQ(fake_backend.Name(), "FakeNodeClient");
  EXPECT_EQ(fake_backend.GetBackendAddress(), "abc");

  // Call a function with the default handler.
  ASSERT_OK(fake_backend.GetCpuInfo({}));

  // Override the default handler and check that it works.
  int call_count = 0;
  fake_backend.handlers().get_cpu_info = [&](auto) {
    call_count++;
    return GetCpuInfoResponse();
  };
  ASSERT_OK(fake_backend.GetCpuInfo({}));
  EXPECT_EQ(call_count, 1);

  // Override the default handler to inject errors.
  fake_backend.handlers().get_cpu_info = [&](auto) {
    return absl::StatusCode::kUnknown;
  };
  EXPECT_FALSE(fake_backend.GetCpuInfo({}).ok());
}

TEST(FakeBackendTest, FakeHostAdapter) {
  // This test just makes sure we can use the fake host adapter, and that it
  // satisfies the NodeClient interface.
  FakeNodeClient fake_backend;
  fake_backend.host_adapter().SetReadError("testfile",
                                           absl::StatusCode::kUnknown);

  HostAdapter &adapter =
      static_cast<NodeClientInterface &>(fake_backend).host_adapter();
  EXPECT_FALSE(adapter.Read("testfile").ok());
}

TEST(FakeBackendTest, CallAllHandlers) {
  FakeNodeClient fake_backend;

// This macro simplifies testing all the various fake handlers.
// It overrides the given handler with the fake one, and tests if it is used.
#define OCPDIAG_FAKE_BACKEND_TEST(handler, fake_handler) \
  {                                                     \
    int cnt = 0;                                        \
    fake_backend.handlers().fake_handler = [&](auto) {  \
      cnt++;                                            \
      return handler##Response();                       \
    };                                                  \
    EXPECT_OK(fake_backend.handler({}));                \
    EXPECT_EQ(cnt, 1);                                  \
  }

  OCPDIAG_FAKE_BACKEND_TEST(GetCpuInfo, get_cpu_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetMemoryInfo, get_memory_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetSensors, get_sensors);
  OCPDIAG_FAKE_BACKEND_TEST(GetFanInfo, get_fan_info);
  OCPDIAG_FAKE_BACKEND_TEST(SetFanPwm, set_fan_pwm);
  OCPDIAG_FAKE_BACKEND_TEST(SetFanZone, set_fan_zone);
  OCPDIAG_FAKE_BACKEND_TEST(GetEepromInfo, get_eeprom_info);
  OCPDIAG_FAKE_BACKEND_TEST(ReadEeprom, read_eeprom);
  OCPDIAG_FAKE_BACKEND_TEST(GetErrors, get_errors);
  OCPDIAG_FAKE_BACKEND_TEST(Reboot, reboot);
  OCPDIAG_FAKE_BACKEND_TEST(SetDevicePower, set_device_power);
  OCPDIAG_FAKE_BACKEND_TEST(GetDevicePower, get_device_power);
  OCPDIAG_FAKE_BACKEND_TEST(GetHwInfo, get_hw_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetSystemInfo, get_system_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetVersion, get_version);
  OCPDIAG_FAKE_BACKEND_TEST(GetNodeInfo, get_node_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetPcieInfo, get_pcie_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetUsbInfo, get_usb_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetUpiInfo, get_upi_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetI2cInfo, get_i2c_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetScsiInfo, get_scsi_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetNvLinkInfo, get_nvlink_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetBootNumber, get_boot_number);
  OCPDIAG_FAKE_BACKEND_TEST(MemoryConvert, memory_convert);
  OCPDIAG_FAKE_BACKEND_TEST(GetSecurityChipInfo, get_security_chip_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetEcInfo, get_ec_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetFirmwareInfo, get_firmware_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetNetworkInterfaceInfo, get_network_interface_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetStorageInfo, get_storage_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetPsuInfo, get_psu_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetGpuInfo, get_gpu_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetNcsiInfo, get_ncsi_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetAcceleratorInfo, get_accelerator_info);
  OCPDIAG_FAKE_BACKEND_TEST(GetStatus, get_status);
}

}  // namespace
}  // namespace ocpdiag::hwinterface
