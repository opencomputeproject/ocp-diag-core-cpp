// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/client/fake_node_client.h"

#include "absl/log/log.h"
#include "absl/status/status.h"
#include "ocpdiag/core/hwinterface/service.pb.h"

#define RUN_FAKE_HANDLER(rpc_name)                                        \
  auto resp_variant = handlers_.rpc_name(req);                            \
  if (resp_variant.index() == 0) {                                        \
    absl::Status status(std::get<0>(resp_variant), "Injected Error");     \
    LOG(INFO) << "Fake HWI '" << GetBackendAddress() << "': " #rpc_name "(" \
            << req.DebugString() << ") = " << status;                     \
    return status;                                                        \
  }                                                                       \
  auto resp = std::get<1>(resp_variant);                                  \
  LOG(INFO) << "Fake HWI '" << GetBackendAddress() << "': " #rpc_name "("   \
          << req.DebugString() << ") = '" << resp.DebugString() << "'";   \
  return resp

namespace ocpdiag::hwinterface {

absl::StatusOr<GetCpuInfoResponse> FakeNodeClient::GetCpuInfo(
    const GetCpuInfoRequest& req) {
  RUN_FAKE_HANDLER(get_cpu_info);
}

absl::StatusOr<SetFanPwmResponse> FakeNodeClient::SetFanPwm(
    const SetFanPwmRequest& req) {
  RUN_FAKE_HANDLER(set_fan_pwm);
}

absl::StatusOr<GetMemoryInfoResponse> FakeNodeClient::GetMemoryInfo(
    const GetMemoryInfoRequest& req) {
  RUN_FAKE_HANDLER(get_memory_info);
}

absl::StatusOr<GetSensorsResponse> FakeNodeClient::GetSensors(
    const GetSensorsRequest& req) {
  RUN_FAKE_HANDLER(get_sensors);
}

absl::StatusOr<GetFanInfoResponse> FakeNodeClient::GetFanInfo(
    const GetFanInfoRequest& req) {
  RUN_FAKE_HANDLER(get_fan_info);
}

absl::StatusOr<SetFanZoneResponse> FakeNodeClient::SetFanZone(
    const SetFanZoneRequest& req) {
  RUN_FAKE_HANDLER(set_fan_zone);
}

absl::StatusOr<GetEepromInfoResponse> FakeNodeClient::GetEepromInfo(
    const GetEepromInfoRequest& req) {
  RUN_FAKE_HANDLER(get_eeprom_info);
}

absl::StatusOr<ReadEepromResponse> FakeNodeClient::ReadEeprom(
    const ReadEepromRequest& req) {
  RUN_FAKE_HANDLER(read_eeprom);
}

absl::StatusOr<GetErrorsResponse> FakeNodeClient::GetErrors(
    const GetErrorsRequest& req) {
  RUN_FAKE_HANDLER(get_errors);
}

absl::StatusOr<RebootResponse> FakeNodeClient::Reboot(
    const RebootRequest& req) {
  RUN_FAKE_HANDLER(reboot);
}

absl::StatusOr<SetDevicePowerResponse> FakeNodeClient::SetDevicePower(
    const SetDevicePowerRequest& req) {
  RUN_FAKE_HANDLER(set_device_power);
}

absl::StatusOr<GetDevicePowerResponse> FakeNodeClient::GetDevicePower(
    const GetDevicePowerRequest& req) {
  RUN_FAKE_HANDLER(get_device_power);
}

absl::StatusOr<GetHwInfoResponse> FakeNodeClient::GetHwInfo(
    const GetHwInfoRequest& req) {
  RUN_FAKE_HANDLER(get_hw_info);
}

absl::StatusOr<GetSystemInfoResponse> FakeNodeClient::GetSystemInfo(
    const GetSystemInfoRequest& req) {
  RUN_FAKE_HANDLER(get_system_info);
}

absl::StatusOr<GetVersionResponse> FakeNodeClient::GetVersion(
    const GetVersionRequest& req) {
  RUN_FAKE_HANDLER(get_version);
}

absl::StatusOr<GetNodeInfoResponse> FakeNodeClient::GetNodeInfo(
    const GetNodeInfoRequest& req) {
  RUN_FAKE_HANDLER(get_node_info);
}

absl::StatusOr<GetPcieInfoResponse> FakeNodeClient::GetPcieInfo(
    const GetPcieInfoRequest& req) {
  RUN_FAKE_HANDLER(get_pcie_info);
}

absl::StatusOr<GetUsbInfoResponse> FakeNodeClient::GetUsbInfo(
    const GetUsbInfoRequest& req) {
  RUN_FAKE_HANDLER(get_usb_info);
}

absl::StatusOr<GetUpiInfoResponse> FakeNodeClient::GetUpiInfo(
    const GetUpiInfoRequest& req) {
  RUN_FAKE_HANDLER(get_upi_info);
}

absl::StatusOr<GetI2cInfoResponse> FakeNodeClient::GetI2cInfo(
    const GetI2cInfoRequest& req) {
  RUN_FAKE_HANDLER(get_i2c_info);
}

absl::StatusOr<GetScsiInfoResponse> FakeNodeClient::GetScsiInfo(
    const GetScsiInfoRequest& req) {
  RUN_FAKE_HANDLER(get_scsi_info);
}

absl::StatusOr<GetNvLinkInfoResponse> FakeNodeClient::GetNvLinkInfo(
    const GetNvLinkInfoRequest& req) {
  RUN_FAKE_HANDLER(get_nvlink_info);
}

absl::StatusOr<GetBootNumberResponse> FakeNodeClient::GetBootNumber(
    const GetBootNumberRequest& req) {
  RUN_FAKE_HANDLER(get_boot_number);
}

absl::StatusOr<MemoryConvertResponse> FakeNodeClient::MemoryConvert(
    const MemoryConvertRequest& req) {
  RUN_FAKE_HANDLER(memory_convert);
}

absl::StatusOr<GetSecurityChipInfoResponse> FakeNodeClient::GetSecurityChipInfo(
    const GetSecurityChipInfoRequest& req) {
  RUN_FAKE_HANDLER(get_security_chip_info);
}

absl::StatusOr<GetEcInfoResponse> FakeNodeClient::GetEcInfo(
    const GetEcInfoRequest& req) {
  RUN_FAKE_HANDLER(get_ec_info);
}

absl::StatusOr<GetFirmwareInfoResponse> FakeNodeClient::GetFirmwareInfo(
    const GetFirmwareInfoRequest& req) {
  RUN_FAKE_HANDLER(get_firmware_info);
}

absl::StatusOr<GetNetworkInterfaceInfoResponse>
FakeNodeClient::GetNetworkInterfaceInfo(
    const GetNetworkInterfaceInfoRequest& req) {
  RUN_FAKE_HANDLER(get_network_interface_info);
}

absl::StatusOr<GetStorageInfoResponse> FakeNodeClient::GetStorageInfo(
    const GetStorageInfoRequest& req) {
  RUN_FAKE_HANDLER(get_storage_info);
}

absl::StatusOr<GetPsuInfoResponse> FakeNodeClient::GetPsuInfo(
    const GetPsuInfoRequest& req) {
  RUN_FAKE_HANDLER(get_psu_info);
}

absl::StatusOr<GetGpuInfoResponse> FakeNodeClient::GetGpuInfo(
    const GetGpuInfoRequest& req) {
  RUN_FAKE_HANDLER(get_gpu_info);
}

absl::StatusOr<GetNcsiInfoResponse> FakeNodeClient::GetNcsiInfo(
    const GetNcsiInfoRequest& req) {
  RUN_FAKE_HANDLER(get_ncsi_info);
}

absl::StatusOr<GetAcceleratorInfoResponse> FakeNodeClient::GetAcceleratorInfo(
    const GetAcceleratorInfoRequest& req) {
  RUN_FAKE_HANDLER(get_accelerator_info);
}

absl::StatusOr<GetStatusResponse> FakeNodeClient::GetStatus(
    const GetStatusRequest& req) {
  RUN_FAKE_HANDLER(get_status);
}

}  // namespace ocpdiag::hwinterface
