// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_HWINTERFACE_BACKENDS_LIB_HW_INFO_H_
#define OCPDIAG_CORE_HWINTERFACE_BACKENDS_LIB_HW_INFO_H_

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "ocpdiag/core/hwinterface/backends/lib/utils.h"
#include "ocpdiag/core/hwinterface/identifier.pb.h"

namespace ocpdiag::hwinterface {

// GetReverseIdMap would create a reverse mapping of the provided id_map.
// It is helpful in GetHwInfo, where we need to map the passed global id
// to local id before processing.
absl::flat_hash_map<std::string, std::string> GetReverseIdMap(
    absl::flat_hash_map<std::string, std::string>& id_map);

// UpdateIdToGlobal updates the local id to global id based on
// the provided local_to_gloabl_id_map
void UpdateIdToGlobal(
    const absl::flat_hash_map<std::string, std::string>* id_map,
    std::string& local_id);

// UpdateIdToLocal updates the global id to local id based on the
// provided global_to_local_id_map
void UpdateIdToLocal(
    const absl::flat_hash_map<std::string, std::string>* id_map,
    std::string& global_id);

// Populates field `id` for a ocpdiag::hwinterface::Identifier. Converts local to
// global hw component id if a match is found in the id_map.
void PopulateId(
    Identifier& identifier,
    const absl::flat_hash_map<std::string, std::string>* id_map = nullptr);

// Returns identifier from unique string `id`.
absl::StatusOr<Identifier> UnmarshalIdentifier(absl::string_view id);

}  // namespace ocpdiag::hwinterface

#endif  // OCPDIAG_CORE_HWINTERFACE_BACKENDS_LIB_HW_INFO_H_
