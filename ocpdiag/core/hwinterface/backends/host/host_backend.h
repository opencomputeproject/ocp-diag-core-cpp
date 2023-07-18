// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_HWINTERFACE_BACKENDS_HOST_HOST_BACKEND_H_
#define OCPDIAG_CORE_HWINTERFACE_BACKENDS_HOST_HOST_BACKEND_H_

#include <cassert>
#include <memory>
#include <string>
#include <utility>

#include "absl/status/statusor.h"
#include "ecclesia/lib/smbios/reader.h"
#include "ocpdiag/core/hwinterface/backends/lib/host_adapter.h"
#include "ocpdiag/core/hwinterface/config.pb.h"
#include "ocpdiag/core/hwinterface/service.pb.h"
#include "ocpdiag/core/hwinterface/service_interface.h"

namespace ocpdiag::hwinterface {

namespace internal {

// This class implements the methods of OCPDiagServiceInterface via
// host diagnostic interface.
// It is not meant to be called directly, instead please use the OCPDiag
// Service Client.
class HostBackend : public OCPDiagServiceInterface {
 public:
  explicit HostBackend(const EntityConfiguration& config,
                       std::unique_ptr<HostAdapter> host_adapter)
      : config_(config), host_adapter_(std::move(host_adapter)) {
    assert(host_adapter_.get());
  }
  static absl::StatusOr<std::unique_ptr<OCPDiagServiceInterface>> Create(
      const EntityConfiguration& config);

  std::string Name() final { return "HostBackend"; }

  std::string GetBackendAddress() final {
    return config_.entity().host_address();
  }

  //
  absl::StatusOr<GetCpuInfoResponse> GetCpuInfo(
      const GetCpuInfoRequest& req) final;

  absl::StatusOr<GetMemoryInfoResponse> GetMemoryInfo(
      const GetMemoryInfoRequest& req) final;

  absl::StatusOr<GetErrorsResponse> GetErrors(
      const GetErrorsRequest& req) final;

  absl::StatusOr<GetHwInfoResponse> GetHwInfo(
      const GetHwInfoRequest& req) final;

  absl::StatusOr<RebootResponse> Reboot(const RebootRequest& req) final;

  absl::StatusOr<MemoryConvertResponse> MemoryConvert(
      const MemoryConvertRequest& req) final;

  absl::StatusOr<GetStorageInfoResponse> GetStorageInfo(
      const GetStorageInfoRequest& req) final;

  absl::StatusOr<GetNodeInfoResponse> GetNodeInfo(
      const GetNodeInfoRequest& req) final;

 private:
  EntityConfiguration config_;
  std::unique_ptr<HostAdapter> host_adapter_;

  absl::StatusOr<ecclesia::SmbiosReader*> GetSmbiosReader();
  std::unique_ptr<ecclesia::SmbiosReader> smbios_reader_;
};

}  // namespace internal

}  // namespace ocpdiag::hwinterface

#endif  // OCPDIAG_CORE_HWINTERFACE_BACKENDS_HOST_HOST_BACKEND_H_
