// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_HWINTERFACE_NODE_CLIENT_H_
#define OCPDIAG_CORE_HWINTERFACE_NODE_CLIENT_H_

#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/flags/flag.h"
#include "absl/status/statusor.h"
#include "ocpdiag/core/hwinterface/backends/lib/host_adapter.h"
#include "ocpdiag/core/hwinterface/config.pb.h"
#include "ocpdiag/core/hwinterface/service.pb.h"
#include "ocpdiag/core/hwinterface/service_flags.h"
#include "ocpdiag/core/hwinterface/service_interface.h"

namespace ocpdiag::hwinterface {

namespace internal {

// Default mapping of backend type to the builder for that backend type.
const absl::flat_hash_map<
    BackendType,
    std::function<absl::StatusOr<std::shared_ptr<OCPDiagServiceInterface>>(
        const EntityConfiguration&)>>&
DefaultBuilderResolver();

}  //  namespace internal

// Gets the ocpdiag::hwinterface::EntityConfigurations proto from a
// JSON-formatted text file passed by hwinterface_config_file_path flag.
// The proto should contain one or more configurations, each of which describes
// the backends configuration of that arena.
// Please see details in ocpdiag::hwinterface::EntityConfigurations comments.
absl::StatusOr<EntityConfigurations> LoadConfigs();

// Gets one ocpdiag::hwinterface::EntityConfiguration from the
// ocpdiag::hwinterface::EntityConfigurations returned from LoadConfigs().
// If LoadConfigs() returns more than one configurations, this library will
// return an internal error.
// This API only applies to single-node diags that targets one arena.
absl::StatusOr<EntityConfiguration> LoadConfig();

// Defines the interface provided by the NodeClient.
class NodeClientInterface : public OCPDiagServiceInterface {
 public:
  // Returns an adapter for interacting directly with the node.
  virtual HostAdapter& host_adapter() = 0;

  // Returns the entity configuration for this node.
  virtual const EntityConfiguration& config() = 0;
};

// NodeClient is the main concrete provider of the OCPDiagServiceInterface.
// It is responsible for instantiating hardware-facing backends, and dispatching
// API calls to the correct backend, specified by the config input.
class NodeClient : public NodeClientInterface {
 public:
  // A method that provides a backend or status code as to why the backend
  // cannot be built.
  using BuilderType =
      std::function<absl::StatusOr<std::shared_ptr<OCPDiagServiceInterface>>(
          const EntityConfiguration&)>;

  // Constructs a NodeClient. If a config is not provided, then one will be
  // loaded from flags. `builder_resolver` maps where methods that will
  // initialize the backends are, and should typically only be overriden in
  // testing. Note that this function can only be applied when LoadConfigs()
  // returns one node configuration. Otherwise an error will be returned.
  static absl::StatusOr<std::unique_ptr<NodeClient>> Create(
      std::optional<EntityConfiguration> config = std::nullopt,
      const absl::flat_hash_map<BackendType, BuilderType>& builder_resolver =
          internal::DefaultBuilderResolver());

  // Constructs NodeClients, or propagates construction failure status,
  // with default configs and builder resolver.
  static absl::StatusOr<std::vector<std::unique_ptr<NodeClient>>>
  CreateClients();

  // Handle the config overrides from flags in service_flags.h
  virtual void HandleConfigOverrides() {}

  HostAdapter& host_adapter() override { return *host_adapter_; }

  const EntityConfiguration& config() override { return config_; }

  std::string Name() override { return "NodeClient"; }

  std::string GetBackendAddress() override {
    return config_.entity().host_address();
  }

  // Gets CPU information. The request can optionally filter by fields but
  // returns all data by default.
  absl::StatusOr<GetCpuInfoResponse> GetCpuInfo(
      const GetCpuInfoRequest& req) override;

  // Gets Memory information. The request can optionally filter by fields but
  // returns all data by default.
  absl::StatusOr<GetMemoryInfoResponse> GetMemoryInfo(
      const GetMemoryInfoRequest& req) override;

  // Enumerates sensors. The request can optionally filter for specific sensor
  // types, but returns all types by default.
  absl::StatusOr<GetSensorsResponse> GetSensors(
      const GetSensorsRequest& req) override;

  // Gets RAS errors. The request can optionally filter for specific error
  // types, but returns all types by default.
  absl::StatusOr<GetErrorsResponse> GetErrors(
      const GetErrorsRequest& req) override;

  // Clear RAS errors.
  absl::StatusOr<ClearErrorsResponse> ClearErrors(
      const ClearErrorsRequest& req) override;

  // Gets fan infromation. The request can optionally filter for specific fan,
  // but returns all types by default.
  absl::StatusOr<GetFanInfoResponse> GetFanInfo(
      const GetFanInfoRequest& req) override;

  // Sets fan PWM.
  absl::StatusOr<SetFanPwmResponse> SetFanPwm(
      const SetFanPwmRequest& req) override;

  // Sets fan Zone.
  absl::StatusOr<SetFanZoneResponse> SetFanZone(
      const SetFanZoneRequest& req) override;

  // Gets info about Eeproms.
  absl::StatusOr<GetEepromInfoResponse> GetEepromInfo(
      const GetEepromInfoRequest& req) override;

  // Reads an Eeprom.
  absl::StatusOr<ReadEepromResponse> ReadEeprom(
      const ReadEepromRequest& req) override;

  // Reboot target machine
  absl::StatusOr<RebootResponse> Reboot(const RebootRequest& req) override;

  // Set the device power off/on
  absl::StatusOr<SetDevicePowerResponse> SetDevicePower(
      const SetDevicePowerRequest& req) override;

  // Get the device power state
  absl::StatusOr<GetDevicePowerResponse> GetDevicePower(
      const GetDevicePowerRequest& req) override;

  // Get target machine status
  absl::StatusOr<GetStatusResponse> GetStatus(
      const GetStatusRequest& req) override;

  // Gets hardware info.
  absl::StatusOr<GetHwInfoResponse> GetHwInfo(
      const GetHwInfoRequest& req) override;

  // Gets system info.
  absl::StatusOr<GetSystemInfoResponse> GetSystemInfo(
      const GetSystemInfoRequest& req) override;

  // Gets version.
  absl::StatusOr<GetVersionResponse> GetVersion(
      const GetVersionRequest& req) override;

  // Gets Node info.
  absl::StatusOr<GetNodeInfoResponse> GetNodeInfo(
      const GetNodeInfoRequest& req) override;

  // Gets pcie info.
  absl::StatusOr<GetPcieInfoResponse> GetPcieInfo(
      const GetPcieInfoRequest& req) override;

  // Gets usb info.
  absl::StatusOr<GetUsbInfoResponse> GetUsbInfo(
      const GetUsbInfoRequest& req) override;

  // Gets UPI/QPI info.
  absl::StatusOr<GetUpiInfoResponse> GetUpiInfo(
      const GetUpiInfoRequest& req) override;

  // Gets I2C/smbus info.
  absl::StatusOr<GetI2cInfoResponse> GetI2cInfo(
      const GetI2cInfoRequest& req) override;

  // Gets SCSI info.
  absl::StatusOr<GetScsiInfoResponse> GetScsiInfo(
      const GetScsiInfoRequest& req) override;

  // Gets NvLink info.
  absl::StatusOr<GetNvLinkInfoResponse> GetNvLinkInfo(
      const GetNvLinkInfoRequest& req) override;

  // Gets boot number.
  absl::StatusOr<GetBootNumberResponse> GetBootNumber(
      const GetBootNumberRequest& req) override;

  // Converts Memory physical address to dimm identifier.
  absl::StatusOr<MemoryConvertResponse> MemoryConvert(
      const MemoryConvertRequest& req) override;

  // Gets Security Chip info.
  absl::StatusOr<GetSecurityChipInfoResponse> GetSecurityChipInfo(
      const GetSecurityChipInfoRequest& req) override;

  // Gets EC info.
  absl::StatusOr<GetEcInfoResponse> GetEcInfo(
      const GetEcInfoRequest& req) override;

  // Gets firmware info.
  absl::StatusOr<GetFirmwareInfoResponse> GetFirmwareInfo(
      const GetFirmwareInfoRequest& req) override;

  // Gets network interface info.
  absl::StatusOr<GetNetworkInterfaceInfoResponse> GetNetworkInterfaceInfo(
      const GetNetworkInterfaceInfoRequest& req) override;

  // Gets storage interface info.
  absl::StatusOr<GetStorageInfoResponse> GetStorageInfo(
      const GetStorageInfoRequest& req) override;

  // Gets psu interface info.
  absl::StatusOr<GetPsuInfoResponse> GetPsuInfo(
      const GetPsuInfoRequest& req) override;

  // Gets GPU info.
  absl::StatusOr<GetGpuInfoResponse> GetGpuInfo(
      const GetGpuInfoRequest& req) override;

  // Gets NCSI info.
  absl::StatusOr<GetNcsiInfoResponse> GetNcsiInfo(
      const GetNcsiInfoRequest& req) override;

  // Gets accelerator info.
  absl::StatusOr<GetAcceleratorInfoResponse> GetAcceleratorInfo(
      const GetAcceleratorInfoRequest& req) override;

 protected:
  // Constructs a NodeClient with the given host adapter.
  // This is 'protected' to allow the MockNodeClient to call it.
  NodeClient(std::unique_ptr<HostAdapter> host_adapter)
      : host_adapter_(std::move(host_adapter)) {}

 private:
  NodeClient(const EntityConfiguration& config);

  absl::Status AddBackend(BackendType backend_type, BuilderType builder);

  std::shared_ptr<OCPDiagServiceInterface> const& SelectBackend(
      BackendMethod method);

  std::unique_ptr<HostAdapter> host_adapter_;
  EntityConfiguration config_;
  absl::flat_hash_map<BackendType, std::shared_ptr<OCPDiagServiceInterface>>
      backends_;
};

}  // namespace ocpdiag::hwinterface

#endif  // OCPDIAG_CORE_HWINTERFACE_NODE_CLIENT_H_
