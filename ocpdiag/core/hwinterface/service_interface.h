// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_HWINTERFACE_SERVICE_INTERFACE_H_
#define OCPDIAG_CORE_HWINTERFACE_SERVICE_INTERFACE_H_

#include "absl/status/statusor.h"
#include "ocpdiag/core/hwinterface/config.pb.h"
#include "ocpdiag/core/hwinterface/service.pb.h"
#include "ocpdiag/core/results/data_model/legacy_results.pb.h"

namespace ocpdiag::hwinterface {

class OCPDiagServiceInterface {
 public:
  virtual ~OCPDiagServiceInterface() = default;

  // Interface name.
  virtual std::string Name() = 0;

  // Gets Backend Address.
  virtual std::string GetBackendAddress() = 0;

  // Gets CPU information.
  // Please see details in ocpdiag/core/hwinterface/service.proto
  // and ocpdiag/core/hwinterface/cpu.proto
  virtual absl::StatusOr<GetCpuInfoResponse> GetCpuInfo(
      const GetCpuInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetCpuInfo is unimplemented by ", Name()));
  }

  // Gets Memory information.
  // Please see details in ocpdiag/core/hwinterface/service.proto
  // and ocpdiag/core/hwinterface/memory.proto
  virtual absl::StatusOr<GetMemoryInfoResponse> GetMemoryInfo(
      const GetMemoryInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetMemoryInfo is unimplemented by ", Name()));
  }

  // Gets sensors information.
  // Please see details in ocpdiag/core/hwinterface/service.proto
  // and ocpdiag/core/hwinterface/sensor.proto
  virtual absl::StatusOr<GetSensorsResponse> GetSensors(
      const GetSensorsRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetSensors is unimplemented by ", Name()));
  }

  // Gets errors information.
  // Please see details in ocpdiag/core/hwinterface/service.proto
  // and ocpdiag/core/hwinterface/error.proto
  virtual absl::StatusOr<GetErrorsResponse> GetErrors(
      const GetErrorsRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetErrors is unimplemented by ", Name()));
  }

  // Clear errors.
  // Please see details in ocpdiag/core/hwinterface/service.proto
  // and ocpdiag/core/hwinterface/error.proto
  virtual absl::StatusOr<ClearErrorsResponse> ClearErrors(
      const ClearErrorsRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("ClearErrors is unimplemented by ", Name()));
  }

  // Gets fan information.
  // Please see details in ocpdiag/core/hwinterface/service.proto
  // and ocpdiag/core/hwinterface/fan.proto
  virtual absl::StatusOr<GetFanInfoResponse> GetFanInfo(
      const GetFanInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetFanInfo is unimplemented by ", Name()));
  }

  // Sets fan pwm.
  // Please see details in ocpdiag/core/hwinterface/service.proto.
  virtual absl::StatusOr<SetFanPwmResponse> SetFanPwm(
      const SetFanPwmRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("SetFanPwm is unimplemented by ", Name()));
  }

  // Sets fan zone.
  // Please see details in ocpdiag/core/hwinterface/service.proto.
  virtual absl::StatusOr<SetFanZoneResponse> SetFanZone(
      const SetFanZoneRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("SetFanZone is unimplemented by ", Name()));
  }

  // Fetches information about Eeproms.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/eeprom.proto.
  virtual absl::StatusOr<GetEepromInfoResponse> GetEepromInfo(
      const GetEepromInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetEepromInfo is unimplemented by ", Name()));
  }

  // Reads an Eeprom.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/eeprom.proto.
  virtual absl::StatusOr<ReadEepromResponse> ReadEeprom(
      const ReadEepromRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("ReadEeprom is unimplemented by ", Name()));
  }

  virtual absl::StatusOr<RebootResponse> Reboot(const RebootRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("Reboot is unimplemented by ", Name()));
  }

  // Set the device power off/on.
  virtual absl::StatusOr<SetDevicePowerResponse> SetDevicePower(
      const SetDevicePowerRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("SetDevicePower is unimplemented by ", Name()));
  }

  // Get the device power state.
  virtual absl::StatusOr<GetDevicePowerResponse> GetDevicePower(
      const GetDevicePowerRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetDevicePower is unimplemented by ", Name()));
  }

  virtual absl::StatusOr<GetStatusResponse> GetStatus(
      const GetStatusRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetStatus is unimplemented by ", Name()));
  }

  // Gets hardware info.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/identifier.proto
  virtual absl::StatusOr<GetHwInfoResponse> GetHwInfo(
      const GetHwInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetHwInfo is unimplemented by ", Name()));
  }

  // Gets system info.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/system.proto
  virtual absl::StatusOr<GetSystemInfoResponse> GetSystemInfo(
      const GetSystemInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetSystemInfo is unimplemented by ", Name()));
  }

  // Gets backend versions.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/system.proto
  virtual absl::StatusOr<GetVersionResponse> GetVersion(
      const GetVersionRequest& req) {
    GetVersionResponse response;
    third_party::ocpdiag::results_pb::SoftwareInfo& software =
        *response.add_softwares();
    software.set_name(Name());
    software.set_version("unknown");
    return response;
  }

  // Gets node info.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/node.proto
  virtual absl::StatusOr<GetNodeInfoResponse> GetNodeInfo(
      const GetNodeInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetNodeInfo is unimplemented by ", Name()));
  }

  // Gets ncsi cable info.
  // Please see details in ocpdiag/core/hwinterface/service.proto
  virtual absl::StatusOr<GetNcsiInfoResponse> GetNcsiInfo(
      const GetNcsiInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetNcsiInfo is unimplemented by ", Name()));
  }

  // Gets pcie info.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/interface.proto
  virtual absl::StatusOr<GetPcieInfoResponse> GetPcieInfo(
      const GetPcieInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetPcieInfo is unimplemented by ", Name()));
  }

  // Gets usb info.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/interface.proto
  virtual absl::StatusOr<GetUsbInfoResponse> GetUsbInfo(
      const GetUsbInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetUsbInfo is unimplemented by ", Name()));
  }

  // Gets upi/qpi info.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/interface.proto
  virtual absl::StatusOr<GetUpiInfoResponse> GetUpiInfo(
      const GetUpiInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetUpiInfo is unimplemented by ", Name()));
  }

  // Gets I2C/smbus info.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/interface.proto
  virtual absl::StatusOr<GetI2cInfoResponse> GetI2cInfo(
      const GetI2cInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetI2cInfo is unimplemented by ", Name()));
  }

  // Gets SCSI info.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/interface.proto
  virtual absl::StatusOr<GetScsiInfoResponse> GetScsiInfo(
      const GetScsiInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetScsiInfo is unimplemented by ", Name()));
  }

  // Gets NvLink info.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/interface.proto
  virtual absl::StatusOr<GetNvLinkInfoResponse> GetNvLinkInfo(
      const GetNvLinkInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetNvLinkInfo is unimplemented by ", Name()));
  }

  // Gets boot number.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/interface.proto
  virtual absl::StatusOr<GetBootNumberResponse> GetBootNumber(
      const GetBootNumberRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetBootNumber is unimplemented by ", Name()));
  }

  // Converts Memory physical address to dimm identifier.
  // Please see details in ocpdiag/core/hwinterface/service.proto.
  virtual absl::StatusOr<MemoryConvertResponse> MemoryConvert(
      const MemoryConvertRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("MemoryConvert is unimplemented by ", Name()));
  }

  // Gets securitychip info.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/securitychip.proto
  virtual absl::StatusOr<GetSecurityChipInfoResponse> GetSecurityChipInfo(
      const GetSecurityChipInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetSecurityChipInfo is unimplemented by ", Name()));
  }

  // Gets EC info.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/ec.proto
  virtual absl::StatusOr<GetEcInfoResponse> GetEcInfo(
      const GetEcInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetEcInfo is unimplemented by ", Name()));
  }

  // Gets firmware info.
  // Please see details in ocpdiag/core/hwinterface/service.proto
  virtual absl::StatusOr<GetFirmwareInfoResponse> GetFirmwareInfo(
      const GetFirmwareInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetFirmwareInfo is unimplemented by ", Name()));
  }

  // Gets network interface info.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/networkinterface.proto
  virtual absl::StatusOr<GetNetworkInterfaceInfoResponse>
  GetNetworkInterfaceInfo(const GetNetworkInterfaceInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetNetworkInterfaceInfo is unimplemented by ", Name()));
  }

  // Gets storage info.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/storage.proto
  virtual absl::StatusOr<GetStorageInfoResponse> GetStorageInfo(
      const GetStorageInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetStorageInfo is unimplemented by ", Name()));
  }

  // Gets psu info.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/psu.proto
  virtual absl::StatusOr<GetPsuInfoResponse> GetPsuInfo(
      const GetPsuInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetPsuInfo is unimplemented by ", Name()));
  }

  // Gets GPU info.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/gpu.proto
  virtual absl::StatusOr<GetGpuInfoResponse> GetGpuInfo(
      const GetGpuInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetGpuInfo is unimplemented by ", Name()));
  }

  // Gets accelerator info.
  // Please see details in ocpdiag/core/hwinterface/service.proto and
  // ocpdiag/core/hwinterface/accelerator.proto
  virtual absl::StatusOr<GetAcceleratorInfoResponse> GetAcceleratorInfo(
      const GetAcceleratorInfoRequest& req) {
    return absl::UnimplementedError(
        absl::StrCat("GetAcceleratorInfo is unimplemented by ", Name()));
  }
};

}  // namespace ocpdiag::hwinterface

#endif  // OCPDIAG_CORE_HWINTERFACE_SERVICE_INTERFACE_H_
