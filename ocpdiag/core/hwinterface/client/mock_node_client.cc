// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/client/mock_node_client.h"

#include <memory>

#include "ocpdiag/core/hwinterface/backends/lib/mock_host_adapter.h"
#include "ocpdiag/core/hwinterface/service.pb.h"

namespace ocpdiag::hwinterface::testonly {

using ::testing::Return;

MockNodeClient::MockNodeClient(std::unique_ptr<HostAdapter> host_adapter)
    : NodeClient(host_adapter ? std::move(host_adapter)
                              : std::make_unique<MockHostAdapter>()) {
  ON_CALL(*this, Name).WillByDefault(Return("MockNodeClient"));
  ON_CALL(*this, GetBackendAddress)
      .WillByDefault(Return("MockNodeClient Backend Address"));
  ON_CALL(*this, GetCpuInfo).WillByDefault(Return(GetCpuInfoResponse()));
  ON_CALL(*this, GetMemoryInfo).WillByDefault(Return(GetMemoryInfoResponse()));
  ON_CALL(*this, GetSensors).WillByDefault(Return(GetSensorsResponse()));
  ON_CALL(*this, GetEepromInfo).WillByDefault(Return(GetEepromInfoResponse()));
  ON_CALL(*this, ReadEeprom).WillByDefault(Return(ReadEepromResponse()));
  ON_CALL(*this, GetErrors).WillByDefault(Return(GetErrorsResponse()));
  ON_CALL(*this, GetStatus).WillByDefault(Return(GetStatusResponse()));
  ON_CALL(*this, GetFanInfo).WillByDefault(Return(GetFanInfoResponse()));
  ON_CALL(*this, SetFanPwm).WillByDefault(Return(SetFanPwmResponse()));
  ON_CALL(*this, SetFanZone).WillByDefault(Return(SetFanZoneResponse()));
  ON_CALL(*this, GetHwInfo).WillByDefault(Return(GetHwInfoResponse()));
  ON_CALL(*this, GetSystemInfo).WillByDefault(Return(GetSystemInfoResponse()));
  ON_CALL(*this, GetVersion).WillByDefault(Return(GetVersionResponse()));
  ON_CALL(*this, GetNodeInfo).WillByDefault(Return(GetNodeInfoResponse()));
  ON_CALL(*this, GetPcieInfo).WillByDefault(Return(GetPcieInfoResponse()));
  ON_CALL(*this, GetUsbInfo).WillByDefault(Return(GetUsbInfoResponse()));
  ON_CALL(*this, GetUpiInfo).WillByDefault(Return(GetUpiInfoResponse()));
  ON_CALL(*this, GetI2cInfo).WillByDefault(Return(GetI2cInfoResponse()));
  ON_CALL(*this, GetScsiInfo).WillByDefault(Return(GetScsiInfoResponse()));
  ON_CALL(*this, GetNvLinkInfo).WillByDefault(Return(GetNvLinkInfoResponse()));
  ON_CALL(*this, GetBootNumber).WillByDefault(Return(GetBootNumberResponse()));
  ON_CALL(*this, MemoryConvert).WillByDefault(Return(MemoryConvertResponse()));
  ON_CALL(*this, GetSecurityChipInfo)
      .WillByDefault(Return(GetSecurityChipInfoResponse()));
  ON_CALL(*this, GetEcInfo).WillByDefault(Return(GetEcInfoResponse()));
  ON_CALL(*this, GetFirmwareInfo)
      .WillByDefault(Return(GetFirmwareInfoResponse()));
  ON_CALL(*this, GetNetworkInterfaceInfo)
      .WillByDefault(Return(GetNetworkInterfaceInfoResponse()));
  ON_CALL(*this, GetStorageInfo)
      .WillByDefault(Return(GetStorageInfoResponse()));
  ON_CALL(*this, GetPsuInfo).WillByDefault(Return(GetPsuInfoResponse()));
  ON_CALL(*this, GetNcsiInfo).WillByDefault(Return(GetNcsiInfoResponse()));
  ON_CALL(*this, Reboot).WillByDefault(Return(RebootResponse()));
  ON_CALL(*this, SetDevicePower)
      .WillByDefault(Return(SetDevicePowerResponse()));
  ON_CALL(*this, GetDevicePower)
      .WillByDefault(Return(GetDevicePowerResponse()));
}

}  // namespace ocpdiag::hwinterface::testonly
