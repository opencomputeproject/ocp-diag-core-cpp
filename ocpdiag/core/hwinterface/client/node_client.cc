// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/client/node_client.h"

#include <cstdint>
#include <string>
#include <vector>

#include "google/protobuf/duration.pb.h"
#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "ocpdiag/core/compat/status_macros.h"
#include "ocpdiag/core/hwinterface/backends/host/host_backend.h"
#include "ocpdiag/core/hwinterface/backends/lib/host_adapter.h"
#include "ocpdiag/core/hwinterface/client/lib/logging.h"
#include "ocpdiag/core/lib/json/json_proto_helpers.h"
#include "ocpdiag/core/hwinterface/config.pb.h"
#include "ocpdiag/core/hwinterface/service.pb.h"
#include "ocpdiag/core/hwinterface/service_flags.h"
#include "ocpdiag/core/hwinterface/service_interface.h"

namespace ocpdiag::hwinterface {
namespace internal {

const absl::flat_hash_map<BackendType, NodeClient::BuilderType>&
DefaultBuilderResolver() {
  static const absl::flat_hash_map<BackendType, NodeClient::BuilderType>
      kDefaultMapping = {
          {BACKENDTYPE_HOST, &HostBackend::Create},
      };
  return kDefaultMapping;
}

inline bool IsOobBackend(BackendType backend) {
  if (backend == BackendType::BACKENDTYPE_REDFISH) {
    return true;
  }
  return false;
}

void ApplyOobOverride(EntityConfiguration* config,
                      const NodeOverride& node_override) {
  config->set_default_backend(node_override.backend);
  if (config->default_backend() == BackendType::BACKENDTYPE_REDFISH) {
    if (node_override.port != 0) {
      config->mutable_config_options()->mutable_redfish_options()->set_port(
          node_override.port);
    }
    if (!node_override.target_address.empty()) {
      config->mutable_config_options()
          ->mutable_redfish_options()
          ->set_target_address(node_override.target_address);
    }
    if (node_override.redfish_auth_method !=
        RedfishConfigOptions::AuthMethod::
            RedfishConfigOptions_AuthMethod_DEFAULT) {
      config->mutable_config_options()
          ->mutable_redfish_options()
          ->set_auth_method(node_override.redfish_auth_method);
    }
    if (node_override.redfish_cache_policy.has_value()) {
      if (node_override.redfish_cache_policy->has_no_cache()) {
        config->mutable_config_options()
            ->mutable_redfish_options()
            ->mutable_redfish_cache_policy()
            ->set_no_cache(node_override.redfish_cache_policy->no_cache());
      } else if (node_override.redfish_cache_policy->has_time_based_cache()) {
        config->mutable_config_options()
            ->mutable_redfish_options()
            ->mutable_redfish_cache_policy()
            ->mutable_time_based_cache()
            ->set_seconds(node_override.redfish_cache_policy->time_based_cache()
                              .seconds());
      }
    }
    // Apply RedPath query overrides.
    if (node_override.redfish_query_overrides.has_value()) {
      config->mutable_config_options()
          ->mutable_redfish_options()
          ->mutable_redfish_query_overrides()
          ->CopyFrom(node_override.redfish_query_overrides.value());
    }
  }
}

void ApplyIbOverride(EntityConfiguration* config,
                     const NodeOverride& node_override) {
  // No IB backend requires a port yet.
  if (!node_override.target_address.empty()) {
    config->mutable_entity()->set_host_address(node_override.target_address);
  }
  config->set_default_backend(node_override.backend);
}

void ApplySingleNodeOverride(EntityConfigurations* configs) {
  EntityConfiguration* config;
  if (configs->node_entity_configurations_size() == 0) {
    config = configs->add_node_entity_configurations();
  } else {
    config = configs->mutable_node_entity_configurations(0);
  }
  NodeOverride node_override;
  // Overrides "--hwinterface_default_backend".
  node_override.backend = absl::GetFlag(FLAGS_hwinterface_default_backend);
  node_override.port = 0;  // Set port to 0 to mark it as un-initialized yet
  if (node_override.backend == BACKENDTYPE_UNKNOWN) {
    node_override.backend = BACKENDTYPE_HOST;
    if (config->default_backend() != BACKENDTYPE_UNKNOWN) {
      node_override.backend = config->default_backend();
    }
  }

  // Overrides "--hwinterface_target_address".
  if (std::string addr_override =
          absl::GetFlag(FLAGS_hwinterface_target_address);
      !addr_override.empty()) {
    node_override.target_address = std::move(addr_override);
  }
  // Overrides "--FLAGS_hwinterface_target_port".
  if (int32_t port = absl::GetFlag(FLAGS_hwinterface_target_port); port) {
    node_override.port = port;
  }
  // Overrides "--FLAGS_hwinterface_proxy".
  if (ProxyType proxy = absl::GetFlag(FLAGS_hwinterface_proxy);
      proxy != ProxyType::PROXYTYPE_NONE) {
    config->mutable_entity()->set_proxy(proxy);
  }
  // Overrides "--FLAGS_hwinterface_redfish_auth_method".
  if (RedfishConfigOptions::AuthMethod auth_method =
          absl::GetFlag(FLAGS_hwinterface_redfish_auth_method);
      auth_method != RedfishConfigOptions::AuthMethod::
                         RedfishConfigOptions_AuthMethod_DEFAULT) {
    node_override.redfish_auth_method = auth_method;
  }

  if (std::optional<RedfishConfigOptions::RedfishCachePolicy> cache_policy =
          absl::GetFlag(FLAGS_hwinterface_redfish_cache_policy);
      cache_policy.has_value()) {
    node_override.redfish_cache_policy = cache_policy.value();
  }

  if (std::optional<RedfishConfigOptions::RedfishQueryOverrides>
          query_overrides =
              absl::GetFlag(FLAGS_hwinterface_redfish_query_overrides);
      query_overrides.has_value()) {
    node_override.redfish_query_overrides = query_overrides.value();
  }

  if (IsOobBackend(node_override.backend)) {
    ApplyOobOverride(config, node_override);
  } else {
    ApplyIbOverride(config, node_override);
  }
}

// Updates existing entity configurations from override with format
// <target_address>:<backend>:<port>.
// 1. When backend is an in-band backend, target_address should the host. The
//    library enumerates the `entity.host_address`es, and applies the override
//    to this configuration as default backend.
// 2. When backend is an out-of-band backend, target_address should be the
//    management controllers. The library enumerates the backend target address
//    overrides, then applies the override to that configuration. For now we
//    only provide `redfish` OOB backend whose target address is
//    config_options.redfish_options.target_address.
absl::Status ApplyNodeOverride(EntityConfigurations* configs,
                               const NodeOverride& node_override) {
  bool backend_exist = false;
  // If the override is an OOB backend.
  if (IsOobBackend(node_override.backend)) {
    for (EntityConfiguration& config :
         *configs->mutable_node_entity_configurations()) {
      if (config.config_options().redfish_options().target_address() ==
          node_override.target_address) {
        ApplyOobOverride(&config, node_override);
        backend_exist = true;
      }
    }
    if (!backend_exist) {
      ApplyOobOverride(configs->add_node_entity_configurations(),
                       node_override);
    }
    return absl::OkStatus();
  }
  // If the override is an IB backend.
  for (EntityConfiguration& config :
       *configs->mutable_node_entity_configurations()) {
    if (config.entity().host_address() == node_override.target_address) {
      ApplyIbOverride(&config, node_override);
      backend_exist = true;
    }
  }
  if (!backend_exist) {
    ApplyIbOverride(configs->add_node_entity_configurations(), node_override);
  }
  return absl::OkStatus();
}

// Applies overrides in flag `mhi_config` to the default EntityConfigurations
// object generated from `mhi_config_filepath`.
// Note that this flag can only be used to set default backend, and the library
// doesn't report conflicting overrides, meaning that when mutliple backend is
// set for the same target. The last override will be applied as the default
// backend.
absl::Status ApplyMultiNodeOverrides(EntityConfigurations* configs) {
  if (absl::GetFlag(FLAGS_mhi_config).empty()) {
    return absl::OkStatus();
  }
  for (const NodeOverride& override : absl::GetFlag(FLAGS_mhi_config)) {
    if (absl::Status status = ApplyNodeOverride(configs, override);
        !status.ok()) {
      return status;
    }
  }
  return absl::OkStatus();
}

}  // namespace internal

absl::StatusOr<EntityConfigurations> LoadConfigs() {
  EntityConfigurations configs;
  //
  // update to mhi_config_filepath.
  if (!absl::GetFlag(FLAGS_hwinterface_config_file_path).empty()) {
    absl::Status config_file_status = JsonFileToMessage(
        absl::GetFlag(FLAGS_hwinterface_config_file_path).c_str(), &configs);
    if (!config_file_status.ok()) {
      return config_file_status;
    }
  }
  // Apply multi-node overrides
  if (absl::Status status = internal::ApplyMultiNodeOverrides(&configs);
      !status.ok()) {
    return status;
  }

  // If the configurations have multiple node entities or multi-node overrides
  // are not empty, return the configurations directly.
  // Otherwise proceed and apply the single-node overrides.
  if (!absl::GetFlag(FLAGS_mhi_config).empty()) {
    return configs;
  }
  int node_entity_count = configs.node_entity_configurations_size();
  if (node_entity_count != 0 && node_entity_count != 1) {
    return configs;
  }

  // Apply single-node overrides.
  internal::ApplySingleNodeOverride(&configs);
  return configs;
}

absl::StatusOr<EntityConfiguration> LoadConfig() {
  absl::StatusOr<EntityConfigurations> configs = LoadConfigs();
  if (!configs.ok()) {
    return configs.status();
  }
  if (configs->node_entity_configurations_size() != 1) {
    return absl::InternalError(
        "Zero or more than one node entities are defined");
  }
  return configs->node_entity_configurations()[0];
}

absl::StatusOr<std::unique_ptr<NodeClient>> NodeClient::Create(
    std::optional<EntityConfiguration> config,
    const absl::flat_hash_map<BackendType, NodeClient::BuilderType>&
        builder_resolver) {
  if (!config.has_value()) {
    ASSIGN_OR_RETURN(config, LoadConfig());
  }
  LOG(INFO) << __FUNCTION__ << ": config: " << config->DebugString();
  auto client = absl::WrapUnique(new NodeClient(*config));
  const BackendType default_backend = config->default_backend();
  auto default_builder = builder_resolver.find(default_backend);
  if (default_builder == builder_resolver.end()) {
    return absl::NotFoundError(
        absl::StrCat("Unable to find builder method for backend ",
                     BackendType_Name(default_backend)));
  }
  if (absl::Status status = client->AddBackend(config->default_backend(),
                                               default_builder->second);
      !status.ok()) {
    return status;
  }
  for (const auto& [unused, backend_type] : config->method_configurations()) {
    if (!client->backends_.contains(backend_type)) {
      auto builder = builder_resolver.find(backend_type);
      if (builder == builder_resolver.end()) {
        return absl::NotFoundError(
            absl::StrCat("Unable to find builder method for backend ",
                         BackendType_Name(default_backend)));
      }
      if (absl::Status status =
              client->AddBackend(backend_type, builder->second);
          !status.ok()) {
        return status;
      }
    }
  }
  // Create client.host_adapter
  std::string target_address = config->entity().host_address();
  if (!target_address.empty()) {
    LOG(INFO) << __FUNCTION__
              << ": Create host adapter for: " << target_address;
    ASSIGN_OR_RETURN(client->host_adapter_,
                     RemoteHostAdapter::Create(target_address));
  } else {
    LOG(INFO) << __FUNCTION__ << ": Creating local host adapter";
    client->host_adapter_ = std::make_unique<LocalHostAdapter>();
  }

  // This is to ensure the StatusOr move constructor is invoked.
  return std::move(client);
}

absl::StatusOr<std::vector<std::unique_ptr<NodeClient>>>
NodeClient::CreateClients() {
  ASSIGN_OR_RETURN(EntityConfigurations configs, LoadConfigs());
  std::vector<std::unique_ptr<NodeClient>> clients;
  for (const EntityConfiguration& config :
       configs.node_entity_configurations()) {
    auto service_client = hwinterface::NodeClient::Create(config);
    if (!service_client.ok()) {
      return service_client.status();
    }
    clients.push_back(*std::move(service_client));
  }
  return clients;
}

NodeClient::NodeClient(const EntityConfiguration& config) : config_(config) {}

absl::Status NodeClient::AddBackend(BackendType backend_type,
                                    NodeClient::BuilderType builder) {
  absl::StatusOr<std::shared_ptr<OCPDiagServiceInterface>> backend =
      builder(config_);
  if (!backend.ok()) {
    return backend.status();
  }
  LOG(INFO) << __FUNCTION__ << ": Added backend for type " << backend_type;
  backends_[backend_type] = *backend;
  return absl::OkStatus();
}

std::shared_ptr<OCPDiagServiceInterface> const& NodeClient::SelectBackend(
    BackendMethod method) {
  BackendType& backend_type =
      (*config_.mutable_method_configurations())[method];
  if (backend_type == BACKENDTYPE_UNKNOWN) {
    backend_type = config_.default_backend();
  }
  return backends_.at(backend_type);
}

absl::StatusOr<GetCpuInfoResponse> NodeClient::GetCpuInfo(
    const GetCpuInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_CPU_INFO),
                                 GetCpuInfo, req);
}

absl::StatusOr<GetMemoryInfoResponse> NodeClient::GetMemoryInfo(
    const GetMemoryInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_MEMORY_INFO),
                                 GetMemoryInfo, req);
}

absl::StatusOr<GetSensorsResponse> NodeClient::GetSensors(
    const GetSensorsRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_SENSORS),
                                 GetSensors, req);
}

absl::StatusOr<GetErrorsResponse> NodeClient::GetErrors(
    const GetErrorsRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_ERRORS),
                                 GetErrors, req);
}

absl::StatusOr<ClearErrorsResponse> NodeClient::ClearErrors(
    const ClearErrorsRequest& req) {
  return SelectBackend(BACKENDMETHOD_CLEAR_ERRORS)->ClearErrors(req);
}

absl::StatusOr<GetFanInfoResponse> NodeClient::GetFanInfo(
    const GetFanInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_FAN_INFO),
                                 GetFanInfo, req);
}

absl::StatusOr<SetFanPwmResponse> NodeClient::SetFanPwm(
    const SetFanPwmRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_SET_FAN_PWM),
                                 SetFanPwm, req);
}

absl::StatusOr<SetFanZoneResponse> NodeClient::SetFanZone(
    const SetFanZoneRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_SET_FAN_ZONE),
                                 SetFanZone, req);
}

absl::StatusOr<GetEepromInfoResponse> NodeClient::GetEepromInfo(
    const GetEepromInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_EEPROM_INFO),
                                 GetEepromInfo, req);
}

absl::StatusOr<ReadEepromResponse> NodeClient::ReadEeprom(
    const ReadEepromRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_READ_EEPROM),
                                 ReadEeprom, req);
}

absl::StatusOr<RebootResponse> NodeClient::Reboot(const RebootRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_REBOOT), Reboot,
                                 req);
}

absl::StatusOr<SetDevicePowerResponse> NodeClient::SetDevicePower(
    const SetDevicePowerRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_SET_DEVICE_POWER),
                                 SetDevicePower, req);
}

absl::StatusOr<GetDevicePowerResponse> NodeClient::GetDevicePower(
    const GetDevicePowerRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_DEVICE_POWER),
                                 GetDevicePower, req);
}

absl::StatusOr<GetStatusResponse> NodeClient::GetStatus(
    const GetStatusRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_STATUS),
                                 GetStatus, req);
}

absl::StatusOr<GetHwInfoResponse> NodeClient::GetHwInfo(
    const GetHwInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_HW_INFO),
                                 GetHwInfo, req);
}

absl::StatusOr<GetSystemInfoResponse> NodeClient::GetSystemInfo(
    const GetSystemInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_SYSTEM_INFO),
                                 GetSystemInfo, req);
}

absl::StatusOr<GetVersionResponse> NodeClient::GetVersion(
    const GetVersionRequest& req) {
  GetVersionResponse response;

  for (auto it = backends_.begin(); it != backends_.end(); ++it) {
    ASSIGN_OR_RETURN(GetVersionResponse backend_response,
                     (OCPDIAG_CALL_BACKEND_API(it->second, GetVersion, req)));
    for (const auto& software : backend_response.softwares()) {
      *response.add_softwares() = software;
    }
  }
  return response;
}

absl::StatusOr<GetNodeInfoResponse> NodeClient::GetNodeInfo(
    const GetNodeInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_NODE_INFO),
                                 GetNodeInfo, req);
}

absl::StatusOr<GetPcieInfoResponse> NodeClient::GetPcieInfo(
    const GetPcieInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_PCIE_INFO),
                                 GetPcieInfo, req);
}

absl::StatusOr<GetUsbInfoResponse> NodeClient::GetUsbInfo(
    const GetUsbInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_USB_INFO),
                                 GetUsbInfo, req);
}

absl::StatusOr<GetUpiInfoResponse> NodeClient::GetUpiInfo(
    const GetUpiInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_UPI_INFO),
                                 GetUpiInfo, req);
}

absl::StatusOr<GetI2cInfoResponse> NodeClient::GetI2cInfo(
    const GetI2cInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_I2C_INFO),
                                 GetI2cInfo, req);
}

absl::StatusOr<GetScsiInfoResponse> NodeClient::GetScsiInfo(
    const GetScsiInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_SCSI_INFO),
                                 GetScsiInfo, req);
}

absl::StatusOr<GetNvLinkInfoResponse> NodeClient::GetNvLinkInfo(
    const GetNvLinkInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_NVLINK_INFO),
                                 GetNvLinkInfo, req);
}

absl::StatusOr<GetBootNumberResponse> NodeClient::GetBootNumber(
    const GetBootNumberRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_BOOT_NUMBER),
                                 GetBootNumber, req);
}

absl::StatusOr<MemoryConvertResponse> NodeClient::MemoryConvert(
    const MemoryConvertRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_MEMORY_CONVERT),
                                 MemoryConvert, req);
}

absl::StatusOr<GetSecurityChipInfoResponse> NodeClient::GetSecurityChipInfo(
    const GetSecurityChipInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(
      SelectBackend(BACKENDMETHOD_GET_SECURITY_CHIP_INFO), GetSecurityChipInfo,
      req);
}

absl::StatusOr<GetEcInfoResponse> NodeClient::GetEcInfo(
    const GetEcInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_EC_INFO),
                                 GetEcInfo, req);
}

absl::StatusOr<GetFirmwareInfoResponse> NodeClient::GetFirmwareInfo(
    const GetFirmwareInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_FIRMWARE_INFO),
                                 GetFirmwareInfo, req);
}

absl::StatusOr<GetNetworkInterfaceInfoResponse>
NodeClient::GetNetworkInterfaceInfo(const GetNetworkInterfaceInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(
      SelectBackend(BACKENDMETHOD_GET_NETWORK_INTERFACE_INFO),
      GetNetworkInterfaceInfo, req);
}

absl::StatusOr<GetStorageInfoResponse> NodeClient::GetStorageInfo(
    const GetStorageInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_STORAGE_INFO),
                                 GetStorageInfo, req);
}

absl::StatusOr<GetPsuInfoResponse> NodeClient::GetPsuInfo(
    const GetPsuInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_PSU_INFO),
                                 GetPsuInfo, req);
}

absl::StatusOr<GetGpuInfoResponse> NodeClient::GetGpuInfo(
    const GetGpuInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_GPU_INFO),
                                 GetGpuInfo, req);
}

absl::StatusOr<GetNcsiInfoResponse> NodeClient::GetNcsiInfo(
    const GetNcsiInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(SelectBackend(BACKENDMETHOD_GET_NCSI_INFO),
                                 GetNcsiInfo, req);
}

absl::StatusOr<GetAcceleratorInfoResponse> NodeClient::GetAcceleratorInfo(
    const GetAcceleratorInfoRequest& req) {
  return OCPDIAG_CALL_BACKEND_API(
      SelectBackend(BACKENDMETHOD_GET_ACCELERATOR_INFO), GetAcceleratorInfo,
      req);
}
}  // namespace ocpdiag::hwinterface
