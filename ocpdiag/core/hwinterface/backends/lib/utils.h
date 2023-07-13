// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_HWINTERFACE_BACKENDS_LIB_UTILS_H_
#define OCPDIAG_CORE_HWINTERFACE_BACKENDS_LIB_UTILS_H_
#include <vector>

#include "google/protobuf/repeated_field.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "ocpdiag/core/hwinterface/backends/lib/host_adapter.h"

namespace ocpdiag::hwinterface {
namespace internal {

enum class NodeType {
  kNodeTypeUnknown = 0,
  kNodeTypeCn,  // host compute node
};

// InfoTypeHave checks if type is contained in info_types
// If info_types is empty, always return TRUE.
inline bool InfoTypeHave(const google::protobuf::RepeatedField<int>& info_types,
                         int type) {
  return (info_types.empty() || std::find(info_types.begin(), info_types.end(),
                                          type) != info_types.end());
}

// InfoTypeHaveAny checks if any of types is contained in info_types
// If info_types is empty, always return TRUE like InfoTypeHave.
inline bool InfoTypeHaveAny(const google::protobuf::RepeatedField<int>& info_types,
                            const std::vector<int>& types) {
  for (const auto& type : types) {
    if (InfoTypeHave(info_types, type)) {
      return true;
    }
  }
  return false;
}

// Parse range list into vector<int>
// List shoud be contiguous and strictly in an increasing order.
// Example:
//     ParseRangeList("1,3-5,7,9-10") return {1,3,4,5,7,9,10}
absl::StatusOr<std::vector<int>> ParseRangeList(absl::string_view str);

// ----------------------------------------------------------------------
// HostPortString()
//    Given a host and port, returns a string of the form "host:port" or
//    "[ho:st]:port", depending on whether host contains colons like
//    an IPv6 literal.  If the host is already bracketed, then additional
//    brackets will not be added.
//
//    The behavior of HostPortString when given a NULL host string
//    (including when hpp.first == NULL) is undefined, except that we
//    guarantee it will not crash.
// ----------------------------------------------------------------------
std::string HostPortString(absl::string_view host, uint16_t port);

// ----------------------------------------------------------------------
// GrpcHostPortString()
//    Given a host and port, returns a string of the form "host:port",
//    "[ho:st]:port" or "dns:///host:port", depending on whether host is
//    IPv4, IPv6 or fqdn.
// ----------------------------------------------------------------------
std::string GrpcHostPortString(absl::string_view host, uint16_t port);

struct NodeInfo {
  std::string entity_configuration_tag;  // identifies the type of machine
  std::string hostname;                  // hostname of the node
  std::string entity_tag;  // node_entity_tag (os_domain) of the node
  NodeType node_type;      // type of node: CN/ACC/IMC etc.
  bool operator==(const NodeInfo& other) const {
    return entity_configuration_tag == other.entity_configuration_tag &&
           hostname == other.hostname && entity_tag == other.entity_tag &&
           node_type == other.node_type;
  }
};

// ----------------------------------------------------------------------
// FetchTargetNodeInfo()
//   Given the target_node, returns the NodeInfo struct that identifies the host
//   Internally this function reads from offline NEA and online NEA file to find
//   the corresponding node info.
// ----------------------------------------------------------------------
absl::StatusOr<NodeInfo> FetchTargetNodeInfo(
    absl::string_view target_hostname = "",
    std::optional<absl::string_view> node_entity_path = std::nullopt);

// ----------------------------------------------------------------------
// GetPidOfProcess()
//   Returns the process ID (PID) of the process with the given name or error.
//   Args:
//     process_name: The name of the process to get the PID of.
//     host: The host adapter to use to get the PID.
// ----------------------------------------------------------------------
absl::StatusOr<int> GetPidOfProcess(absl::string_view process_name,
                                    ocpdiag::hwinterface::HostAdapter& host);

// ----------------------------------------------------------------------
// RestartProcess()
//   Restarts the process with the given name.
//   Args:
//     process_name: The name of the process to restart.
//     host: The host adapter to use to restart the process.
//     process_ready_wait: The amount of time to wait for process to restart.
// ----------------------------------------------------------------------
absl::Status RestartProcess(absl::string_view process_name,
                            ocpdiag::hwinterface::HostAdapter& host,
                            absl::Duration process_ready_wait);

}  // namespace internal
}  // namespace ocpdiag::hwinterface
#endif  // OCPDIAG_CORE_HWINTERFACE_BACKENDS_LIB_UTILS_H_
