// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/service_flags.h"

#include <cstdint>
#include <iostream>
#include <optional>
#include <string>

#include "google/protobuf/duration.pb.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/ascii.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "ocpdiag/core/hwinterface/config.pb.h"
#include "re2/re2.h"

ABSL_FLAG(
    std::string, machine_under_test, "local",
    "Machine under test. If the test binary is running on the same machine as "
    "the machine under test, just keep the default \"local\".");

ABSL_FLAG(
    std::vector<ocpdiag::hwinterface::NodeOverride>, mhi_config,
    std::vector<ocpdiag::hwinterface::NodeOverride>{},
    "A <backend_type>://<target_address>:<port> list separated by commas. Only "
    "\"gsys\", \"redfish\", \"mimik_redfish\", \"fake\", or \"host\" are "
    "available "
    "backend options now. This will override the configuration provided in "
    "\"mhi_config_(json|file)\" if it exists. Note that this only applies to "
    "the default backend of the target_address. For more complicated use case "
    "where one EntityConfiguration requires multiple backend types, please use "
    "\"mhi_config_filepath\" flag to pass in the nested JSON object. If IPv6 "
    "target_address is used, please make sure the brackets [] are properly "
    "applied.");

ABSL_FLAG(
    std::string, mhi_config_filepath, "",
    "Path to the config file defining a JSON representation of"
    "ocpdiag::hwinterface::EntityConfigurations. This will override the "
    "configuration provided in \"hwinterface_config_file_path\" if it exists.");

// Single-node hwinterface overrides only.
// DEPRECATED: Use mhi_config and mhi_config_filepath instead.
ABSL_FLAG(std::string, hwinterface_config_file_path, "",
          "Path to the config file.");

// Single-node hwinterface overrides only.
// DEPRECATED: Use mhi_config and mhi_config_filepath instead.
ABSL_FLAG(std::string, hwinterface_target_address, "",
          "The machine/node/arena address that OCPDiag hardware interface "
          "will talk to. If the test binary is running on the same target as "
          "hwinterface_target, just keep the default and the hardware "
          "interface will connect to localhost. Note that this flag will "
          "only override target address of the default backend.");

// Single-node hwinterface overrides only.
// DEPRECATED: Use mhi_config and mhi_config_filepath instead.
ABSL_FLAG(int, hwinterface_target_port, 0,
          "The hwinterface_target port that OCPDiag hardware interface will "
          "talk to. Hardware interface will use the default port for the "
          "backend when this flag is not assigned. Note that this flag will "
          "only override target port of the default backend.");

// Single-node hwinterface overrides only.
// DEPRECATED: Use mhi_config and mhi_config_filepath instead.
ABSL_FLAG(
    ocpdiag::hwinterface::BackendType, hwinterface_default_backend,
    ocpdiag::hwinterface::BackendType::BACKENDTYPE_UNKNOWN,
    "The default backend to use in OCPDiag hardware interface. Only \"gsys\", "
    "\"redfish\", \"mimik_redfish\", or \"host\" are available now. This will "
    "override the configuration provided in \"hwinterface_config_file_path\" "
    "if it exists. ");

// Single-node hwinterface overrides only.
ABSL_FLAG(
    ocpdiag::hwinterface::RedfishConfigOptions::AuthMethod,
    hwinterface_redfish_auth_method,
    ocpdiag::hwinterface::RedfishConfigOptions::AuthMethod::
        RedfishConfigOptions_AuthMethod_DEFAULT,
    "The default authentication method to use in OCPDiag hardware interface. "
    "Available options are \"basic\", \"tls\""
    ". By default, it uses BASIC, which refers"
    " to HTTP username+password authentication. ");

ABSL_FLAG(
    std::optional<
        ocpdiag::hwinterface::RedfishConfigOptions::RedfishQueryOverrides>,
    hwinterface_redfish_query_overrides, std::nullopt,
    "A <resource>:<query_id> list separated by commas. Only "
    "\"sensor_query\" is an available resource option now. The \"query_id\" "
    "value "
    "shall override default RedPath query id used by the Redfish backend for "
    "the specified resource.");

ABSL_FLAG(
    std::optional<
        ocpdiag::hwinterface::RedfishConfigOptions::RedfishCachePolicy>,
    hwinterface_redfish_cache_policy, std::nullopt,
    "Cache policy specified using the fromat "
    "<cache_type>:<cache_duration_seconds>. Only \"no_cache\" and "
    "\"time_based_cache\" are available cache types. For \"time_based_cache\" "
    " cache_duration can be specified in seconds using "
    "\"cache_duration_seconds\". If no cachePolicy argument is specified or "
    " an empty string is specified, we will use \"time_based_cache\" with "
    "infinite duration.");

// Multi-node hwinterface overrides only.
ABSL_FLAG(
    ocpdiag::hwinterface::ProxyType, hwinterface_proxy,
    ocpdiag::hwinterface::ProxyType::PROXYTYPE_NONE,
    "In multi-node machine, the node that OCPDiag hardware interface will proxy "
    "to. Only \"something_cn\" is supported now. This will "
    "override the configuration provided in \"hwinterface_config_file_path\" "
    "if it exists. ");

namespace ocpdiag::hwinterface {

// AbslUnparseFlag converts from an AuthMethod to a string.
// Must be in same namespace as AuthMethod.
std::string AbslUnparseFlag(
    const RedfishConfigOptions::AuthMethod auth_method) {
  switch (auth_method) {
    case RedfishConfigOptions_AuthMethod_BASIC:
      return "basic";
    case RedfishConfigOptions_AuthMethod_TLS:
      return "tls";
    default:
      return "default";
  }
}

// AbslParseFlag converts from a string to AuthMethod.
// Must be in same namespace as AuthMethod.
// Input parameters will never be null.
// Same applies to all other AbslParseFlag functions
bool AbslParseFlag(absl::string_view text,
                   RedfishConfigOptions::AuthMethod* auth_method,
                   std::string* error) {
  *auth_method = RedfishConfigOptions_AuthMethod_DEFAULT;
  std::string input_text = absl::AsciiStrToLower(text);
  if (input_text.empty() || input_text == "default") {
    return true;
  }

  if (input_text == "basic") {
    *auth_method = RedfishConfigOptions_AuthMethod_BASIC;
  } else if (input_text == "tls") {
    *auth_method = RedfishConfigOptions_AuthMethod_TLS;
  } else {
    *error = absl::StrFormat(
        "Unimplemented auth method %s specified in flag "
        "hwinterface_redfish_auth_method. Only \"basic\", "
        "\"tls\", "
        "are available now.",
        text);
    return false;
  }
  return true;
}

// AbslUnparseFlag converts from a string to BackendType.
// Must be in same namespace as BackendType.
std::string AbslUnparseFlag(const BackendType backend_type) {
  switch (backend_type) {
    case BackendType::BACKENDTYPE_GSYS:
      return "gsys";
    case BackendType::BACKENDTYPE_REDFISH:
      return "redfish";
    case BackendType::BACKENDTYPE_HOST:
      return "host";
    default:
      return "unknown";
  }
}

// AbslParseFlag converts from a string to BackendType.
// Must be in same namespace as BackendType.
bool AbslParseFlag(absl::string_view text, BackendType* backend_type,
                   std::string* error) {
  *backend_type = BackendType::BACKENDTYPE_UNKNOWN;
  if (text.empty() || text == "unknown") {
    return true;
  }

  if (text == "host") {
    *backend_type = BackendType::BACKENDTYPE_HOST;
  } else if (text == "fake") {
    *backend_type = BackendType::BACKENDTYPE_FAKE;
  } else if (text == "redfish") {
    *backend_type = BackendType::BACKENDTYPE_REDFISH;
  } else if (text == "gsys") {
    *backend_type = BackendType::BACKENDTYPE_GSYS;
  } else {
    *error = absl::StrFormat(
        "Unimplemented backend %s specified in flag "
        "hwinterface_default_backend. Only \"gsys\", "
        "\"redfish\", \"mimik_redfish\" or \"host\" are available now.",
        text);
    return false;
  }
  return true;
}

// AbslUnparseFlag converts from a NodeType to Machine Manager os_domain string.
std::string AbslUnparseFlag(const ProxyType proxy_type) {
  switch (proxy_type) {
    default:
      return "";
  }
}

// AbslParseFlag converts the user input node to NodeType.
// NodeTypes are assigned based on: mmanager/daemon/configs/something.textpb.
bool AbslParseFlag(absl::string_view text, ProxyType* proxy,
                   std::string* error) {
  *proxy = ProxyType::PROXYTYPE_NONE;
  if (text.empty() || text == "local") {
    return true;
  }
  std::string text_lower = absl::AsciiStrToLower(text);
  return true;
}

// AbslUnparseFlag converts from a std::vector<NodeOverride> to string.
std::string AbslUnparseFlag(const std::vector<NodeOverride> overrides) {
  static constexpr absl::string_view kNodeOverrideFormat = "%s://%s:%d";
  std::vector<std::string> text_overrides;
  text_overrides.reserve(overrides.size());
  for (const NodeOverride& override : overrides) {
    std::string text_override =
        absl::StrFormat(kNodeOverrideFormat, AbslUnparseFlag(override.backend),
                        override.target_address, override.port);
    text_overrides.push_back(std::move(text_override));
  }
  return absl::StrJoin(text_overrides, ",");
}

absl::StatusOr<NodeOverride> ParseOverride(absl::string_view override) {
  static const RE2 kNodeOverrideRegex = RE2(R"((.+)://(.+):(\d+):?(.+)?)");
  NodeOverride node_override;
  std::string backend;
  std::string auth_method;  // currently point to a redfish backend auth method

  if (!RE2::FullMatch(override, kNodeOverrideRegex, &backend,
                      &node_override.target_address, &node_override.port,
                      &auth_method)) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "Failed to parse string \"%s\" in flag \"mhi_config\". Please make "
        "sure it's formatted as "
        "<backend_type>://<target_address>:<port>[:<auth>]",
        override));
  }
  if (std::string error;
      !AbslParseFlag(backend, &node_override.backend, &error)) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "Failed to get backend type in override \"%s\": %s", override, error));
  }
  if (!auth_method.empty()) {
    std::string error;
    if (!AbslParseFlag(auth_method, &node_override.redfish_auth_method,
                       &error)) {
      return absl::InvalidArgumentError(absl::StrFormat(
          "Failed to get auth method in override \"%s\": %s", override, error));
    }
  }
  return node_override;
}

// AbslParseFlag converts from a string to std::vector<NodeOverride>.
bool AbslParseFlag(absl::string_view text, std::vector<NodeOverride>* overrides,
                   std::string* error) {
  if (text.empty()) {
    return true;
  }
  for (absl::string_view text_override : absl::StrSplit(text, ',')) {
    absl::StatusOr<NodeOverride> override = ParseOverride(text_override);
    if (!override.ok()) {
      if (error != nullptr) {
        *error += override.status().message();
      }
      return false;
    }
    overrides->push_back(*std::move(override));
  }
  return true;
}

// AbslUnparseFlag converts from a RedfishQueryOverrides to string.
std::string AbslUnparseFlag(
    const ocpdiag::hwinterface::RedfishConfigOptions::RedfishQueryOverrides
        redfish_query_overrides) {
  static constexpr absl::string_view kQueryOverrideFormat = "%s:%s";
  std::vector<std::string> text_overrides;

  // Handle sensor query override
  if (absl::string_view sensor_query = redfish_query_overrides.sensor_query();
      !sensor_query.empty()) {
    std::string text_override =
        absl::StrFormat(kQueryOverrideFormat, "sensor_query", sensor_query);
    text_overrides.push_back(std::move(text_override));
  }
  return absl::StrJoin(text_overrides, ",");
}

// AbslParseFlag converts from a string to RedfishQueryOverrides.
bool AbslParseFlag(
    absl::string_view text,
    ocpdiag::hwinterface::RedfishConfigOptions::RedfishQueryOverrides*
        redfish_query_overrides,
    std::string* error) {
  static const RE2 kQueryOverrideRegex = RE2(R"((.+):(.+))");
  if (text.empty()) {
    return true;
  }

  std::string resource_type, query_id;
  for (absl::string_view text_override : absl::StrSplit(text, ',')) {
    if (!RE2::FullMatch(text_override, kQueryOverrideRegex, &resource_type,
                        &query_id)) {
      *error += absl::StrFormat(
          "Failed to parse string \"%s\" in flag "
          "\"hwinterface_redfish_query_overrides\". "
          "Please make sure it's formatted as  <resource>:<query_id>",
          std::string(text_override));
      return false;
    }
    if (resource_type == "sensor_query") {
      *redfish_query_overrides->mutable_sensor_query() = query_id;
    }
  }
  return true;
}

// AbslUnparseFlag converts from a RedfishCachePolicy to string.
std::string AbslUnparseFlag(
    const ocpdiag::hwinterface::RedfishConfigOptions::RedfishCachePolicy
        redfish_cache_policy) {
  static constexpr absl::string_view kCachePolicyFormat = "%s:%d";

  if (redfish_cache_policy.has_no_cache()) {
    return "no_cache:";
  }
  return absl::StrFormat(kCachePolicyFormat, "time_based_cache",
                         redfish_cache_policy.time_based_cache().seconds());
}

// AbslParseFlag converts from a string to RedfishCachePolicy.
bool AbslParseFlag(
    absl::string_view text,
    ocpdiag::hwinterface::RedfishConfigOptions::RedfishCachePolicy*
        redfish_cache_policy,
    std::string* error) {
  static constexpr LazyRE2 kRedfishCachePolicyRegex = {
      R"(^(\w+)(?:\:(\d+))?$)"};
  if (text.empty()) {
    return true;
  }

  std::string cache_type, parsed_cache_duration;
  int64_t cache_duration_seconds;
  if (!RE2::FullMatch(text, *kRedfishCachePolicyRegex, &cache_type,
                      &parsed_cache_duration)) {
    *error += absl::StrFormat(
        "Failed to parse string \"%s\" in flag "
        "\"hwinterface_redfish_cache_policy\". "
        "Please make sure it's formatted as  "
        "<cache_type>:<cache_duration_seconds>.",
        std::string(text));
    return false;
  }
  if (cache_type == "time_based_cache" &&
      absl::SimpleAtoi(parsed_cache_duration, &cache_duration_seconds)) {
    (*redfish_cache_policy->mutable_time_based_cache())
        .set_seconds(cache_duration_seconds);
  } else if (cache_type == "no_cache") {
    redfish_cache_policy->set_no_cache(
        RedfishConfigOptions::RedfishCachePolicy::NO_CACHE);
  }
  return true;
}

}  // namespace ocpdiag::hwinterface
