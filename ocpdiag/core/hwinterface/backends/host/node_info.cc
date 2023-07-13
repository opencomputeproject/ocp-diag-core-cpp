// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "ocpdiag/core/compat/status_macros.h"
#include "ocpdiag/core/hwinterface/backends/host/host_backend.h"
#include "ocpdiag/core/hwinterface/backends/lib/utils.h"
#include "ocpdiag/core/hwinterface/identifier.pb.h"
#include "ocpdiag/core/hwinterface/node.pb.h"
#include "ocpdiag/core/hwinterface/service.pb.h"
#include "ocpdiag/core/hwinterface/storage.pb.h"

namespace ocpdiag::hwinterface::internal {

namespace {}  // namespace

absl::StatusOr<GetNodeInfoResponse> HostBackend::GetNodeInfo(
    const GetNodeInfoRequest& req) {
  GetNodeInfoResponse resp;

  // Currently, only storage devices are included in the FRUS infotype.
  if (InfoTypeHave(req.info_types(), node::InfoType::FRUS)) {
    GetStorageInfoRequest storage_info_req;
    storage_info_req.add_info_types(storage::InfoType::IDENTIFIER);
    ASSIGN_OR_RETURN_WITH_MESSAGE(GetStorageInfoResponse storage_info_resp,
                                  GetStorageInfo(storage_info_req),
                                  "GetStorageInfo failed.");

    for (const storage::Info& storage_info : storage_info_resp.info()) {
      node::Fru& fru = *resp.mutable_info()->add_frus();
      *fru.mutable_identifier() = storage_info.id();
    }
  }

  return resp;
}

}  // namespace ocpdiag::hwinterface::internal
