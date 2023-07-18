// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_HWINTERFACE_BACKENDS_FAKE_FAKE_NODE_CLIENT_H_
#define OCPDIAG_CORE_HWINTERFACE_BACKENDS_FAKE_FAKE_NODE_CLIENT_H_

#include <exception>

#include "absl/status/statusor.h"
#include "ocpdiag/core/hwinterface/backends/lib/fake_host_adapter.h"
#include "ocpdiag/core/hwinterface/client/node_client.h"
#include "ocpdiag/core/hwinterface/config.pb.h"
#include "ocpdiag/core/hwinterface/service.pb.h"

namespace ocpdiag::hwinterface {

// This class provides a fake backend instance. You can override any handlers
// with the handlers() property.
class FakeNodeClient : public NodeClientInterface {
 public:
  // Stores all the handlers for the fake backend implementation. You can
  // override any of these by setting them directly. The default handlers all
  // return successfully with empty protos.
  struct Handlers {
    // Prevent copying this struct to avoid added complexity. Sometimes pybind
    // copies this when it's misconfigured, which creates hard to debug
    // scenarios.
    Handlers() = default;
    Handlers(const Handlers&) = delete;
    Handlers& operator=(const Handlers&) = delete;

    // Notice that the fake handlers return a std::variant which is either a
    // status code or a response proto. The reason we don't use a StatusOr in
    // this case is because it needs to be wrapped in Python, and pybind does
    // not have conversions for StatusOr objects (it wants to convert non-OK
    // statuses to exceptions). So in order to make this work properly we have
    // to return a status code to inject errors instead.
    std::function<std::variant<absl::StatusCode, GetCpuInfoResponse>(
        const GetCpuInfoRequest&)>
        get_cpu_info = [](auto) { return GetCpuInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetMemoryInfoResponse>(
        const GetMemoryInfoRequest&)>
        get_memory_info = [](auto) { return GetMemoryInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetSensorsResponse>(
        const GetSensorsRequest&)>
        get_sensors = [](auto) { return GetSensorsResponse(); };

    std::function<std::variant<absl::StatusCode, GetFanInfoResponse>(
        const GetFanInfoRequest&)>
        get_fan_info = [](auto) { return GetFanInfoResponse(); };

    std::function<std::variant<absl::StatusCode, SetFanPwmResponse>(
        const SetFanPwmRequest&)>
        set_fan_pwm = [](auto) { return SetFanPwmResponse(); };

    std::function<std::variant<absl::StatusCode, SetFanZoneResponse>(
        const SetFanZoneRequest&)>
        set_fan_zone = [](auto) { return SetFanZoneResponse(); };

    std::function<std::variant<absl::StatusCode, GetEepromInfoResponse>(
        const GetEepromInfoRequest&)>
        get_eeprom_info = [](auto) { return GetEepromInfoResponse(); };

    std::function<std::variant<absl::StatusCode, ReadEepromResponse>(
        const ReadEepromRequest&)>
        read_eeprom = [](auto) { return ReadEepromResponse(); };

    std::function<std::variant<absl::StatusCode, GetErrorsResponse>(
        const GetErrorsRequest&)>
        get_errors = [](auto) { return GetErrorsResponse(); };

    std::function<std::variant<absl::StatusCode, RebootResponse>(
        const RebootRequest&)>
        reboot = [](auto) { return RebootResponse(); };

    std::function<std::variant<absl::StatusCode, SetDevicePowerResponse>(
        const SetDevicePowerRequest&)>
        set_device_power = [](auto) { return SetDevicePowerResponse(); };

    std::function<std::variant<absl::StatusCode, GetDevicePowerResponse>(
        const GetDevicePowerRequest&)>
        get_device_power = [](auto) { return GetDevicePowerResponse(); };

    std::function<std::variant<absl::StatusCode, GetHwInfoResponse>(
        const GetHwInfoRequest&)>
        get_hw_info = [](auto) { return GetHwInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetSystemInfoResponse>(
        const GetSystemInfoRequest&)>
        get_system_info = [](auto) { return GetSystemInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetVersionResponse>(
        const GetVersionRequest&)>
        get_version = [](auto) { return GetVersionResponse(); };

    std::function<std::variant<absl::StatusCode, GetNodeInfoResponse>(
        const GetNodeInfoRequest&)>
        get_node_info = [](auto) { return GetNodeInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetPcieInfoResponse>(
        const GetPcieInfoRequest& req)>
        get_pcie_info = [](auto) { return GetPcieInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetUsbInfoResponse>(
        const GetUsbInfoRequest&)>
        get_usb_info = [](auto) { return GetUsbInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetUpiInfoResponse>(
        const GetUpiInfoRequest&)>
        get_upi_info = [](auto) { return GetUpiInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetI2cInfoResponse>(
        const GetI2cInfoRequest&)>
        get_i2c_info = [](auto) { return GetI2cInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetScsiInfoResponse>(
        const GetScsiInfoRequest&)>
        get_scsi_info = [](auto) { return GetScsiInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetNvLinkInfoResponse>(
        const GetNvLinkInfoRequest&)>
        get_nvlink_info = [](auto) { return GetNvLinkInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetBootNumberResponse>(
        const GetBootNumberRequest&)>
        get_boot_number = [](auto) { return GetBootNumberResponse(); };

    std::function<std::variant<absl::StatusCode, MemoryConvertResponse>(
        const MemoryConvertRequest&)>
        memory_convert = [](auto) { return MemoryConvertResponse(); };

    std::function<std::variant<absl::StatusCode, GetSecurityChipInfoResponse>(
        const GetSecurityChipInfoRequest&)>
        get_security_chip_info =
            [](auto) { return GetSecurityChipInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetEcInfoResponse>(
        const GetEcInfoRequest&)>
        get_ec_info = [](auto) { return GetEcInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetFirmwareInfoResponse>(
        const GetFirmwareInfoRequest&)>
        get_firmware_info = [](auto) { return GetFirmwareInfoResponse(); };

    std::function<
        std::variant<absl::StatusCode, GetNetworkInterfaceInfoResponse>(
            const GetNetworkInterfaceInfoRequest&)>
        get_network_interface_info =
            [](auto) { return GetNetworkInterfaceInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetStorageInfoResponse>(
        const GetStorageInfoRequest&)>
        get_storage_info = [](auto) { return GetStorageInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetPsuInfoResponse>(
        const GetPsuInfoRequest&)>
        get_psu_info = [](auto) { return GetPsuInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetGpuInfoResponse>(
        const GetGpuInfoRequest&)>
        get_gpu_info = [](auto) { return GetGpuInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetNcsiInfoResponse>(
        const GetNcsiInfoRequest&)>
        get_ncsi_info = [](auto) { return GetNcsiInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetAcceleratorInfoResponse>(
        const GetAcceleratorInfoRequest&)>
        get_accelerator_info =
            [](auto) { return GetAcceleratorInfoResponse(); };

    std::function<std::variant<absl::StatusCode, GetStatusResponse>(
        const GetStatusRequest&)>
        get_status = [](auto) { return GetStatusResponse(); };
  };

  // Constructs a new FakeNodeClient instance.
  FakeNodeClient(const EntityConfiguration& config = {}) : config_(config) {}
  ~FakeNodeClient() override = default;

  // Simplify the class by avoiding copying/moving.
  FakeNodeClient(const FakeNodeClient&) = delete;
  FakeNodeClient& operator=(const FakeNodeClient&) = delete;

  // Returns the fake handlers so you can override them.
  Handlers& handlers() { return handlers_; }

  // Returns a fake host adapter that lets you customize the behavior.
  FakeHostAdapter& host_adapter() override { return host_adapter_; }

  const EntityConfiguration& config() { return config_; }

  // Below is just the OCPDiagServiceInterface implementation, which calls our
  // fake handlers.

  std::string Name() final { return "FakeNodeClient"; }

  std::string GetBackendAddress() final {
    return config_.entity().host_address();
  }

  absl::StatusOr<GetCpuInfoResponse> GetCpuInfo(
      const GetCpuInfoRequest& req) final;

  absl::StatusOr<GetMemoryInfoResponse> GetMemoryInfo(
      const GetMemoryInfoRequest& req) final;

  absl::StatusOr<GetSensorsResponse> GetSensors(
      const GetSensorsRequest& req) final;

  absl::StatusOr<GetFanInfoResponse> GetFanInfo(
      const GetFanInfoRequest& req) final;

  absl::StatusOr<SetFanPwmResponse> SetFanPwm(
      const SetFanPwmRequest& req) final;

  absl::StatusOr<SetFanZoneResponse> SetFanZone(
      const SetFanZoneRequest& req) final;

  absl::StatusOr<GetEepromInfoResponse> GetEepromInfo(
      const GetEepromInfoRequest& req) final;

  absl::StatusOr<ReadEepromResponse> ReadEeprom(
      const ReadEepromRequest& req) final;

  absl::StatusOr<GetErrorsResponse> GetErrors(
      const GetErrorsRequest& req) final;

  absl::StatusOr<RebootResponse> Reboot(const RebootRequest& req) final;

  absl::StatusOr<SetDevicePowerResponse> SetDevicePower(
      const SetDevicePowerRequest& req) final;

  absl::StatusOr<GetDevicePowerResponse> GetDevicePower(
      const GetDevicePowerRequest& req) final;

  absl::StatusOr<GetHwInfoResponse> GetHwInfo(
      const GetHwInfoRequest& req) final;

  absl::StatusOr<GetSystemInfoResponse> GetSystemInfo(
      const GetSystemInfoRequest& req) final;

  absl::StatusOr<GetVersionResponse> GetVersion(
      const GetVersionRequest& req) final;

  absl::StatusOr<GetNodeInfoResponse> GetNodeInfo(
      const GetNodeInfoRequest& req) final;

  absl::StatusOr<GetPcieInfoResponse> GetPcieInfo(
      const GetPcieInfoRequest& req) final;

  absl::StatusOr<GetUsbInfoResponse> GetUsbInfo(
      const GetUsbInfoRequest& req) final;

  absl::StatusOr<GetUpiInfoResponse> GetUpiInfo(
      const GetUpiInfoRequest& req) final;

  absl::StatusOr<GetI2cInfoResponse> GetI2cInfo(
      const GetI2cInfoRequest& req) final;

  absl::StatusOr<GetScsiInfoResponse> GetScsiInfo(
      const GetScsiInfoRequest& req) final;

  absl::StatusOr<GetNvLinkInfoResponse> GetNvLinkInfo(
      const GetNvLinkInfoRequest& req) final;

  absl::StatusOr<GetBootNumberResponse> GetBootNumber(
      const GetBootNumberRequest& req) final;

  absl::StatusOr<MemoryConvertResponse> MemoryConvert(
      const MemoryConvertRequest& req) final;

  absl::StatusOr<GetSecurityChipInfoResponse> GetSecurityChipInfo(
      const GetSecurityChipInfoRequest& req) final;

  absl::StatusOr<GetEcInfoResponse> GetEcInfo(
      const GetEcInfoRequest& req) final;

  absl::StatusOr<GetFirmwareInfoResponse> GetFirmwareInfo(
      const GetFirmwareInfoRequest& req) final;

  absl::StatusOr<GetNetworkInterfaceInfoResponse> GetNetworkInterfaceInfo(
      const GetNetworkInterfaceInfoRequest& req) final;

  absl::StatusOr<GetStorageInfoResponse> GetStorageInfo(
      const GetStorageInfoRequest& req) final;

  absl::StatusOr<GetPsuInfoResponse> GetPsuInfo(
      const GetPsuInfoRequest& req) final;

  absl::StatusOr<GetGpuInfoResponse> GetGpuInfo(
      const GetGpuInfoRequest& req) final;

  absl::StatusOr<GetNcsiInfoResponse> GetNcsiInfo(
      const GetNcsiInfoRequest& req) final;

  absl::StatusOr<GetAcceleratorInfoResponse> GetAcceleratorInfo(
      const GetAcceleratorInfoRequest& req) final;

  absl::StatusOr<GetStatusResponse> GetStatus(
      const GetStatusRequest& req) final;

 private:
  const EntityConfiguration config_;
  Handlers handlers_;
  FakeHostAdapter host_adapter_;
};

}  // namespace ocpdiag::hwinterface

#endif  // OCPDIAG_CORE_HWINTERFACE_BACKENDS_FAKE_FAKE_NODE_CLIENT_H_
