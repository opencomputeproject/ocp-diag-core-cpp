// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_HWINTERFACE_CLIENT_MOCK_SERVICE_CLIENT_H_
#define OCPDIAG_CORE_HWINTERFACE_CLIENT_MOCK_SERVICE_CLIENT_H_

#include "gmock/gmock.h"
#include "ocpdiag/core/hwinterface/client/node_client.h"
#include "ocpdiag/core/hwinterface/service.pb.h"

namespace ocpdiag::hwinterface::testonly {

class MockNodeClient : public ocpdiag::hwinterface::NodeClient {
 public:
  // You can optionally inject your own HostAdapter. Leave it empty to use a
  // default mock.
  MockNodeClient(std::unique_ptr<HostAdapter> host_adapter = nullptr);

  MOCK_METHOD(std::string, Name, (), (final));
  MOCK_METHOD(std::string, GetBackendAddress, (), (final));
  MOCK_METHOD(absl::StatusOr<GetCpuInfoResponse>, GetCpuInfo,
              (const GetCpuInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetMemoryInfoResponse>, GetMemoryInfo,
              (const GetMemoryInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetSensorsResponse>, GetSensors,
              (const GetSensorsRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetEepromInfoResponse>, GetEepromInfo,
              (const GetEepromInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<ReadEepromResponse>, ReadEeprom,
              (const ReadEepromRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetErrorsResponse>, GetErrors,
              (const GetErrorsRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetStatusResponse>, GetStatus,
              (const GetStatusRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<ClearErrorsResponse>, ClearErrors,
              (const ClearErrorsRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetFanInfoResponse>, GetFanInfo,
              (const GetFanInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<SetFanPwmResponse>, SetFanPwm,
              (const SetFanPwmRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<SetFanZoneResponse>, SetFanZone,
              (const SetFanZoneRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetHwInfoResponse>, GetHwInfo,
              (const GetHwInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetSystemInfoResponse>, GetSystemInfo,
              (const GetSystemInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetVersionResponse>, GetVersion,
              (const GetVersionRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetNodeInfoResponse>, GetNodeInfo,
              (const GetNodeInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetPcieInfoResponse>, GetPcieInfo,
              (const GetPcieInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetUsbInfoResponse>, GetUsbInfo,
              (const GetUsbInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetUpiInfoResponse>, GetUpiInfo,
              (const GetUpiInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetI2cInfoResponse>, GetI2cInfo,
              (const GetI2cInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetScsiInfoResponse>, GetScsiInfo,
              (const GetScsiInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetNvLinkInfoResponse>, GetNvLinkInfo,
              (const GetNvLinkInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetBootNumberResponse>, GetBootNumber,
              (const GetBootNumberRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<MemoryConvertResponse>, MemoryConvert,
              (const MemoryConvertRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetSecurityChipInfoResponse>, GetSecurityChipInfo,
              (const GetSecurityChipInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetEcInfoResponse>, GetEcInfo,
              (const GetEcInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetFirmwareInfoResponse>, GetFirmwareInfo,
              (const GetFirmwareInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetNetworkInterfaceInfoResponse>,
              GetNetworkInterfaceInfo,
              (const GetNetworkInterfaceInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetStorageInfoResponse>, GetStorageInfo,
              (const GetStorageInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetPsuInfoResponse>, GetPsuInfo,
              (const GetPsuInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetGpuInfoResponse>, GetGpuInfo,
              (const GetGpuInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetNcsiInfoResponse>, GetNcsiInfo,
              (const GetNcsiInfoRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<RebootResponse>, Reboot,
              (const RebootRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<SetDevicePowerResponse>, SetDevicePower,
              (const SetDevicePowerRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetDevicePowerResponse>, GetDevicePower,
              (const GetDevicePowerRequest& req), (final));
  MOCK_METHOD(absl::StatusOr<GetAcceleratorInfoResponse>, GetAcceleratorInfo,
              (const GetAcceleratorInfoRequest& req), (final));
};

}  // namespace ocpdiag::hwinterface::testonly

#endif  // OCPDIAG_CORE_HWINTERFACE_CLIENT_MOCK_SERVICE_CLIENT_H_
