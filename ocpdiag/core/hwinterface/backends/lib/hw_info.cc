// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/backends/lib/hw_info.h"

#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/log/log.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_replace.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "ocpdiag/core/compat/status_macros.h"
#include "ocpdiag/core/hwinterface/backends/lib/utils.h"
#include "ocpdiag/core/hwinterface/identifier.pb.h"

namespace ocpdiag::hwinterface {

using ocpdiag::hwinterface::internal::NodeInfo;
using ocpdiag::hwinterface::internal::NodeType;

absl::flat_hash_map<std::string, std::string> GetReverseIdMap(
    absl::flat_hash_map<std::string, std::string>& id_map) {
  absl::flat_hash_map<std::string, std::string> reverse_map = [&]() {
    absl::flat_hash_map<std::string, std::string> global_to_local_id_map;
    global_to_local_id_map.reserve(id_map.size());
    for (const auto& [k, v] : id_map) {
      // We only add reverse mapping for local devpath, no need to do it
      // for stable id.
      if (!absl::StartsWith(k, "/phys")) {
        continue;
      }
      global_to_local_id_map[v] = k;
    }
    return global_to_local_id_map;
  }();
  return reverse_map;
}

void UpdateIdToLocal(
    const absl::flat_hash_map<std::string, std::string>* id_map,
    std::string& global_id) {
  if (id_map) {
    // Do longest prefix match to find the local devpath:
    // e.g., for a devpath of /phys/a/b/c:d:e, we try finding the following
    // in sequence: /phys/a/b/c:d:e, /phys/a/b/c, /phys/a/b, /phys/a, /phys
    std::size_t found = global_id.length();
    std::string prefix = global_id;
    while (found != std::string::npos && found != 0) {
      prefix = prefix.substr(0, found);
      std::string remainder = global_id.substr(found);
      if (id_map->find(prefix) != id_map->end()) {
        global_id = id_map->at(prefix);
        absl::StrAppend(&global_id, remainder);
        return;
      }
      if (found = prefix.find_first_of(':'); found != std::string::npos) {
        continue;
      }
      found = prefix.find_last_of('/');
    }
  }
}

void UpdateIdToGlobal(
    const absl::flat_hash_map<std::string, std::string>* id_map,
    std::string& local_id) {
  if (id_map) {
    if (id_map->find(local_id) != id_map->end()) {
      local_id = id_map->at(local_id);
    }
  }
}

void PopulateId(Identifier& identifier,
                const absl::flat_hash_map<std::string, std::string>* id_map) {
  UpdateIdToGlobal(id_map, *identifier.mutable_devpath());
  identifier.set_id(absl::StrJoin(
      {
          identifier.devpath(),
          identifier.odata_id(),
          identifier.name(),
          identifier.arena(),
          identifier.type(),
      },
      "%"));
}

void PopulateIdForGsys(
    Identifier& identifier, const NodeInfo& node_info,
    const absl::flat_hash_map<std::string, std::string>* id_map) {
  PopulateId(identifier, id_map);
}

absl::StatusOr<Identifier> UnmarshalIdentifier(absl::string_view id) {
  Identifier identifier;
  constexpr int kIdentifierComponentsCount = 5;

  std::vector<absl::string_view> v =
      absl::StrSplit(id, absl::MaxSplits('%', kIdentifierComponentsCount));
  if (v.size() != kIdentifierComponentsCount) {
    return absl::InvalidArgumentError(
        absl::StrFormat("Invalid identifier id format. %s", id));
  }

  identifier.set_id(id.data(), id.size());
  identifier.set_devpath(v[0].data(), v[0].size());
  identifier.set_odata_id(v[1].data(), v[1].size());
  identifier.set_name(v[2].data(), v[2].size());
  identifier.set_arena(v[3].data(), v[3].size());
  identifier.set_type(v[4].data(), v[4].size());
  return identifier;
}

}  // namespace ocpdiag::hwinterface
