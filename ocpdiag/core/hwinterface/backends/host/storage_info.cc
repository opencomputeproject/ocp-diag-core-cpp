// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <cstdio>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/log/log.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/match.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "include/nlohmann/json.hpp"
#include "ocpdiag/core/compat/status_macros.h"
#include "ocpdiag/core/hwinterface/backends/host/host_backend.h"
#include "ocpdiag/core/hwinterface/backends/lib/host_adapter.h"
#include "ocpdiag/core/hwinterface/backends/lib/hw_info.h"
#include "ocpdiag/core/hwinterface/backends/lib/utils.h"
#include "ocpdiag/core/hwinterface/identifier.pb.h"
#include "ocpdiag/core/hwinterface/service.pb.h"
#include "ocpdiag/core/hwinterface/storage.pb.h"

namespace ocpdiag::hwinterface::internal {

namespace {

constexpr absl::Duration kSmartctlTimeout = absl::Seconds(10);
constexpr char kSmartctlDir[] = "/usr/sbin/smartctl";
constexpr char kJsonArg[] = "--json";
constexpr char kScanArg[] = "--scan";
constexpr char kAllArg[] = "--all";
constexpr char kGetNonSmartSettingsArg[] = "-g";
constexpr char kSecuritySettingArg[] = "security";
constexpr char kDevices[] = "devices";
constexpr char kName[] = "name";
constexpr char kType[] = "type";
constexpr char kUserCapacity[] = "user_capacity";
constexpr char kBlocks[] = "blocks";
constexpr char kLogicalBlockSize[] = "logical_block_size";
constexpr char kPhysicalBlockSize[] = "physical_block_size";
constexpr char kRotationRate[] = "rotation_rate";
constexpr char kSataVersion[] = "sata_version";
constexpr char kNvmeVersion[] = "nvme_version";
constexpr char kModelName[] = "model_name";
constexpr char kSerialNumber[] = "serial_number";
constexpr char kSmartSupport[] = "smart_support";
constexpr char kAvailable[] = "available";
constexpr char kEnabled[] = "enabled";
constexpr char kAtaSmartData[] = "ata_smart_data";
constexpr char kCapabilities[] = "capabilities";
constexpr char kAtaSctCapabilities[] = "ata_sct_capabilities";
constexpr char kAtaSecurity[] = "ata_security";
constexpr char kString[] = "string";

absl::StatusOr<nlohmann::basic_json<>> ParseJson(absl::string_view json_str) {
  nlohmann::basic_json<> scan_result =
      nlohmann::json::parse(json_str, nullptr, false);
  if (scan_result.is_discarded()) {
    return absl::InternalError(
        absl::StrCat("Unable to parse text to json. Result from smartctl is "
                     "not in json format. Result: ",
                     json_str));
  }
  return scan_result;
}

storage::DeviceType GetDeviceType(nlohmann::basic_json<>& device_result) {
  if (device_result.contains(kSataVersion)) {
    switch (device_result[kRotationRate].get<int>()) {
      case 0:
        return storage::DEVICETYPE_SSD;
      default:
        return storage::DEVICETYPE_HDD;
    }
  } else if (device_result.contains(kNvmeVersion)) {
    return storage::DEVICETYPE_NVME_SSD;
  }
  LOG(WARNING) << "Unable to determine the type of storage device.";
  return storage::DEVICETYPE_UNKNOWN;
}

// Runs command with given `args` through `host_adapter` and returns the STDOUT.
absl::StatusOr<std::string> RunCommand(
    std::unique_ptr<HostAdapter>& host_adapter,
    const std::vector<std::string>& args) {
  ASSIGN_OR_RETURN(HostAdapter::CommandResult cmd_result,
                   (host_adapter->RunCommand(kSmartctlTimeout, args)));
  if (cmd_result.exit_code == 64) {
    // If a disk has old indications of failure in its SMART log, exit code 64
    // is provided. smartctl is still successfully executed.
    return cmd_result.stdout;
  } else if (cmd_result.exit_code) {
    return absl::InternalError(absl::StrFormat(
        "Command \"%s\" failed. Exit code: %d. Stderr: %s. Stdout: %s",
        absl::StrJoin(args, " "), cmd_result.exit_code, cmd_result.stderr,
        cmd_result.stdout));
  }
  return cmd_result.stdout;
}

std::optional<storage::Capability> CapabilityMapping(
    absl::string_view smartctl_cap_key) {
  static const auto* const kCapabilityMapping =
      new absl::flat_hash_map<std::string, storage::Capability>{
          {"gp_logging", storage::CAP_GPL},
          {"error_recovery_control", storage::CAP_SCT_ERC},
          {"feature_control", storage::CAP_SCT_FEAT},
          {"data_table", storage::CAP_SCT_TAB},
      };
  if (const auto iter = kCapabilityMapping->find(smartctl_cap_key);
      iter != kCapabilityMapping->end()) {
    return iter->second;
  }
  return std::nullopt;
}

// Maps the capability key/value from smartctl JSON output to HWI capability
// and sets it to AtaInfo.
// e.g.:
//
//    "gp_logging_supported": true
//
//    from smartctl would be mapped to HWI storage::CAP_GPL and set to
//    supported_capabilities in AtaInfo.
void MapAndSetCap(const std::string& smartctl_cap_key, const bool cap_status,
                  storage::AtaInfo& ata_info) {
  if (absl::EndsWith(smartctl_cap_key, "_enabled")) {
    std::optional<storage::Capability> cap =
        CapabilityMapping(absl::string_view(smartctl_cap_key.c_str(),
                                            smartctl_cap_key.size() - 8));
    if (cap == std::nullopt) return;
    if (cap_status) {
      ata_info.add_enabled_capabilities(*cap);
      ata_info.add_supported_capabilities(*cap);
    }
  } else if (absl::EndsWith(smartctl_cap_key, "_supported")) {
    std::optional<storage::Capability> cap =
        CapabilityMapping(absl::string_view(smartctl_cap_key.c_str(),
                                            smartctl_cap_key.size() - 10));
    if (cap == std::nullopt) return;
    if (cap_status) {
      ata_info.add_supported_capabilities(*cap);
    }
  }
}

void SetAtaInfo(const std::string& device_name,
                const nlohmann::basic_json<>& device_result,
                storage::AtaInfo& ata_info) {
  ata_info.set_num_sectors(device_result[kUserCapacity][kBlocks]);
  ata_info.set_logical_sector_size(device_result[kLogicalBlockSize]);
  ata_info.set_physical_sector_size(device_result[kPhysicalBlockSize]);
  ata_info.set_rpm(device_result[kRotationRate]);

  if (device_result[kSmartSupport][kAvailable]) {
    ata_info.add_supported_capabilities(storage::CAP_SMART);
  }
  if (device_result[kSmartSupport][kEnabled]) {
    ata_info.add_enabled_capabilities(storage::CAP_SMART);
  }

  // Sets enabled_capabilities and supported_capabilities for AtaInfo.
  for (const auto& [smartctl_cap_key, cap_status] :
       device_result[kAtaSmartData][kCapabilities].items()) {
    if (!cap_status.is_boolean()) continue;
    MapAndSetCap(smartctl_cap_key, cap_status, ata_info);
  }

  if (device_result.contains(kAtaSctCapabilities)) {
    ata_info.add_supported_capabilities(storage::CAP_SCT);
    for (const auto& [smartctl_cap_key, cap_status] :
         device_result[kAtaSctCapabilities].items()) {
      if (!cap_status.is_boolean()) continue;
      MapAndSetCap(smartctl_cap_key, cap_status, ata_info);
    }
  }
}

// Macro to create a pair for mapping from security string from smartctl to HWI
// storage SecurityLevel.
#define SECURITY_LEVEL_PAIR(security_level) \
  { #security_level, storage::SECURITYLEVEL_##security_level }

// Returns a map with smartctl security keyword string as key and HWI
// storage SecurityLevel as value.
const absl::flat_hash_map<std::string, storage::SecurityLevel>&
SecurityLevelMap() {
  static const auto* const kSecurityLevelMap =
      new absl::flat_hash_map<std::string, storage::SecurityLevel>{
          SECURITY_LEVEL_PAIR(SEC0), SECURITY_LEVEL_PAIR(SEC1),
          SECURITY_LEVEL_PAIR(SEC2), SECURITY_LEVEL_PAIR(SEC3),
          SECURITY_LEVEL_PAIR(SEC4), SECURITY_LEVEL_PAIR(SEC5),
          SECURITY_LEVEL_PAIR(SEC6),
      };
  return *kSecurityLevelMap;
}

#undef SECURITY_LEVEL_PAIR

absl::Status SetSecurityInfo(const std::string& device_name,
                             std::unique_ptr<HostAdapter>& host_adapter,
                             storage::SecurityInfo& security_info) {
  ASSIGN_OR_RETURN(
      const std::string& stdout,
      RunCommand(host_adapter, {kSmartctlDir, kJsonArg, kGetNonSmartSettingsArg,
                                kSecuritySettingArg, device_name}));
  ASSIGN_OR_RETURN(nlohmann::basic_json<> security_json, ParseJson(stdout));

  // Only ATA security can be determined for now. An unknown security mode would
  // be shown even if TCG security is enabled.
  if (security_json.contains(kAtaSecurity) &&
      security_json[kAtaSecurity][kEnabled] == true) {
    security_info.set_mode(storage::SECURITYMODE_ATA);

    for (const auto& [sec_keyword, security_level] : SecurityLevelMap()) {
      if (absl::StrContains(
              security_json[kAtaSecurity][kString].get<std::string>(),
              sec_keyword)) {
        security_info.set_level(security_level);
        break;
      }
    }
  }

  return absl::OkStatus();
}

}  // namespace

absl::StatusOr<GetStorageInfoResponse> HostBackend::GetStorageInfo(
    const GetStorageInfoRequest& req) {
  GetStorageInfoResponse resp;

  ASSIGN_OR_RETURN(
      const std::string scan_cmd_stdout,
      RunCommand(host_adapter_, {kSmartctlDir, kJsonArg, kScanArg}));

  ASSIGN_OR_RETURN(nlohmann::basic_json<> scan_result,
                   ParseJson(scan_cmd_stdout));

  for (const auto& device : scan_result[kDevices]) {
    storage::Info& info = *resp.add_info();

    if (InfoTypeHave(req.info_types(), storage::InfoType::SYSTEM_INFO)) {
      info.mutable_system_info()->set_kernel_name(device[kName]);
    }

    nlohmann::basic_json<> device_result;
    if (InfoTypeHave(req.info_types(), storage::InfoType::IDENTIFIER) ||
        InfoTypeHave(req.info_types(), storage::InfoType::DEVICE_TYPE) ||
        InfoTypeHave(req.info_types(), storage::InfoType::ATA_INFO)) {
      ASSIGN_OR_RETURN(const std::string& all_info_cmd_stdout,
                       RunCommand(host_adapter_, {kSmartctlDir, kJsonArg,
                                                  kAllArg, device[kName]}));
      ASSIGN_OR_RETURN(device_result, ParseJson(all_info_cmd_stdout));
    }

    if (InfoTypeHave(req.info_types(), storage::InfoType::IDENTIFIER)) {
      info.mutable_id()->set_name(device_result[kSerialNumber]);
      info.mutable_id()->set_name(
          absl::StrCat(device_result[kModelName].get<std::string>(), "#",
                       device_result[kSerialNumber].get<std::string>()));
      info.mutable_id()->set_type(device[kType]);
      PopulateId(*info.mutable_id());
    }

    if (InfoTypeHave(req.info_types(), storage::InfoType::DEVICE_TYPE)) {
      info.set_device_type(GetDeviceType(device_result));
    }

    if (InfoTypeHave(req.info_types(), storage::InfoType::ATA_INFO)) {
      SetAtaInfo(device[kName], device_result, *info.mutable_ata_info());
    }

    if (InfoTypeHave(req.info_types(), storage::InfoType::SECURITY_INFO)) {
      RETURN_IF_ERROR(SetSecurityInfo(device[kName], host_adapter_,
                                      *info.mutable_security_info()));
    }
  }
  return resp;
}

}  // namespace ocpdiag::hwinterface::internal
