// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_HWINTERFACE_SERVICE_FLAGS_H_
#define OCPDIAG_CORE_HWINTERFACE_SERVICE_FLAGS_H_
#include <optional>
#include <string>
#include <vector>

#include "absl/flags/declare.h"
#include "absl/flags/flag.h"
#include "absl/flags/marshalling.h"
#include "absl/strings/string_view.h"
#include "ocpdiag/core/hwinterface/config.pb.h"

namespace ocpdiag::hwinterface {

struct NodeOverride {
  BackendType backend;
  std::string target_address;
  int port;
  RedfishConfigOptions::AuthMethod redfish_auth_method =
      RedfishConfigOptions_AuthMethod_DEFAULT;
  std::optional<RedfishConfigOptions::RedfishCachePolicy> redfish_cache_policy;
  std::optional<RedfishConfigOptions::RedfishQueryOverrides>
      redfish_query_overrides;
};

bool AbslParseFlag(absl::string_view text, BackendType* backend_type,
                   std::string* error);
bool AbslParseFlag(absl::string_view text,
                   RedfishConfigOptions::AuthMethod* auth_method,
                   std::string* error);
bool AbslParseFlag(absl::string_view text, ProxyType* proxy,
                   std::string* error);
bool AbslParseFlag(absl::string_view text, std::vector<NodeOverride>* overrides,
                   std::string* error);
bool AbslParseFlag(
    absl::string_view text,
    ocpdiag::hwinterface::RedfishConfigOptions::RedfishCachePolicy*
        redfish_cache_policy,
    std::string* error);
bool AbslParseFlag(
    absl::string_view text,
    ocpdiag::hwinterface::RedfishConfigOptions::RedfishQueryOverrides*
        redfish_query_overrides,
    std::string* error);
}  // namespace ocpdiag::hwinterface

ABSL_DECLARE_FLAG(std::string, machine_under_test);
ABSL_DECLARE_FLAG(std::vector<ocpdiag::hwinterface::NodeOverride>, mhi_config);
//
ABSL_DECLARE_FLAG(std::string, mhi_config_filepath);

// DEPRECATED: Use mhi_config and mhi_config_filepath instead.
ABSL_DECLARE_FLAG(std::string, hwinterface_config_file_path);
// DEPRECATED: Use mhi_config and mhi_config_filepath instead.
ABSL_DECLARE_FLAG(std::string, hwinterface_target_address);
// DEPRECATED: Use mhi_config and mhi_config_filepath instead.
ABSL_DECLARE_FLAG(int, hwinterface_target_port);
ABSL_DECLARE_FLAG(ocpdiag::hwinterface::RedfishConfigOptions::AuthMethod,
                  hwinterface_redfish_auth_method);
ABSL_DECLARE_FLAG(
    std::optional<
        ocpdiag::hwinterface::RedfishConfigOptions::RedfishCachePolicy>,
    hwinterface_redfish_cache_policy);
ABSL_DECLARE_FLAG(
    std::optional<
        ocpdiag::hwinterface::RedfishConfigOptions::RedfishQueryOverrides>,
    hwinterface_redfish_query_overrides);

// DEPRECATED: Use mhi_config and mhi_config_filepath instead.
ABSL_DECLARE_FLAG(ocpdiag::hwinterface::BackendType,
                  hwinterface_default_backend);
ABSL_DECLARE_FLAG(ocpdiag::hwinterface::ProxyType, hwinterface_proxy);

#endif  // OCPDIAG_CORE_HWINTERFACE_SERVICE_FLAGS_H_
