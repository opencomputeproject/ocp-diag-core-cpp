// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/backends/host/host_backend.h"

#include <stdint.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "google/protobuf/timestamp.pb.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/ascii.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "ecclesia/lib/smbios/reader.h"
#include "ecclesia/lib/smbios/structures.emb.h"
#include "ecclesia/lib/smbios/system_information.h"
#include "ocpdiag/core/compat/status_macros.h"
#include "ocpdiag/core/hwinterface/backends/host/cpu.h"
#include "ocpdiag/core/hwinterface/backends/host/error.h"
#include "ocpdiag/core/hwinterface/backends/lib/host_adapter.h"
#include "ocpdiag/core/hwinterface/backends/lib/hw_info.h"
#include "ocpdiag/core/hwinterface/backends/lib/utils.h"
#include "ocpdiag/core/hwinterface/config.pb.h"
#include "ocpdiag/core/hwinterface/cpu.pb.h"
#include "ocpdiag/core/hwinterface/error.pb.h"
#include "ocpdiag/core/hwinterface/identifier.pb.h"
#include "ocpdiag/core/hwinterface/service.pb.h"
#include "ocpdiag/core/hwinterface/service_interface.h"

namespace ocpdiag::hwinterface {

namespace internal {

const absl::Duration kSoftRebootTimeout = absl::Minutes(5);
constexpr absl::string_view kDmiTablePath = "/sys/firmware/dmi/tables/DMI";

absl::StatusOr<std::unique_ptr<OCPDiagServiceInterface>> HostBackend::Create(
    const EntityConfiguration& config) {
  if (!config.entity().host_address().empty()) {
    ASSIGN_OR_RETURN(std::unique_ptr<RemoteHostAdapter> remote,
                     RemoteHostAdapter::Create(config.entity().host_address()));
    return std::make_unique<HostBackend>(config, std::move(remote));
  }
  return std::make_unique<HostBackend>(config,
                                       std::make_unique<LocalHostAdapter>());
}

absl::StatusOr<ecclesia::SmbiosReader*> HostBackend::GetSmbiosReader() {
  if (smbios_reader_ == nullptr) {
    ASSIGN_OR_RETURN(const std::string table_data_string,
                     host_adapter_->Read(kDmiTablePath));
    std::vector<uint8_t> table_data{table_data_string.begin(),
                                    table_data_string.end()};
    smbios_reader_ = std::make_unique<ecclesia::SmbiosReader>(table_data);
  }
  return smbios_reader_.get();
}

absl::StatusOr<GetCpuInfoResponse> HostBackend::GetCpuInfo(
    const GetCpuInfoRequest& req) {
  GetCpuInfoResponse resp;
  cpu::Info& info = *resp.mutable_info();
  CpuTopology topology;
  CpuSignature sig;

  if (InfoTypeHave(req.info_types(), cpu::InfoType::TOPOLOGY)) {
    RETURN_IF_ERROR(topology.Gather(*host_adapter_));
    cpu::Topology& t = *info.mutable_topology();
    t.set_sockets_enabled(topology.sockets_count());
    t.set_logical_cores_enabled(topology.logical_cores_count());
    t.set_physical_cores_enabled(topology.physical_cores_count());
    t.set_logical_cores_max_per_socket(topology.logical_cores_max_per_socket());
  }

  if (InfoTypeHave(req.info_types(), cpu::InfoType::SIGNATURE)) {
    RETURN_IF_ERROR(sig.Gather(*host_adapter_));
    cpu::Signature& c = *info.mutable_signature();
    c.set_vendor(sig.vendor());
    c.set_family(sig.family());
    c.set_model(sig.model());
    c.set_stepping(sig.stepping());
  }

  if (InfoTypeHave(req.info_types(), cpu::InfoType::LPU_INFO)) {
    RETURN_IF_ERROR(topology.Gather(*host_adapter_));
    RETURN_IF_ERROR(sig.Gather(*host_adapter_));
    for (const CpuNumaNode& node : topology.numa_nodes()) {
      for (const CpuLpu& host_lpu : node.lpus()) {
        cpu::LpuInfo& lpu = (*info.mutable_lpu_info())[host_lpu.cpu_id()];
        lpu.set_socket_id(host_lpu.socket_id());
        lpu.set_die_id(host_lpu.die_id());
        lpu.set_core_id(host_lpu.core_id());
        lpu.set_thread_id(host_lpu.thread_id());
        lpu.set_numa_node_id(host_lpu.numa_node_id());
        lpu.set_microcode_revision(sig.microcode());
      }
    }
  }

  if (InfoTypeHave(req.info_types(), cpu::InfoType::FREQUENCY)) {
    RETURN_IF_ERROR(topology.Gather(*host_adapter_));
    for (const CpuNumaNode& node : topology.numa_nodes()) {
      for (const CpuLpu& host_lpu : node.lpus()) {
        CpuFrequency cpu_freq(host_lpu.cpu_id());
        RETURN_IF_ERROR(cpu_freq.Gather(*host_adapter_));
        cpu::LpuFrequency& freq =
            (*info.mutable_frequency())[cpu_freq.cpu_id()];
        freq.set_design_freq(cpu_freq.design_freq());
        freq.set_cur_freq(cpu_freq.cur_freq());
        freq.set_max_freq(cpu_freq.max_freq());
      }
    }
  }

  if (InfoTypeHave(req.info_types(), cpu::InfoType::THROTTLE_INFO)) {
    RETURN_IF_ERROR(topology.Gather(*host_adapter_));
    for (const CpuNumaNode& node : topology.numa_nodes()) {
      for (const CpuLpu& host_lpu : node.lpus()) {
        CpuThrottleInfo cpu_throttle(host_lpu.cpu_id());
        RETURN_IF_ERROR(cpu_throttle.Gather(*host_adapter_));
        (*info.mutable_thermal_throttle_count())[cpu_throttle.cpu_id()] =
            cpu_throttle.core_throttle_count();
      }
    }
  }

  if (InfoTypeHave(req.info_types(), cpu::InfoType::LPU_IDENTIFIER)) {
    RETURN_IF_ERROR(topology.Gather(*host_adapter_));
    for (const CpuNumaNode& node : topology.numa_nodes()) {
      for (const CpuLpu& host_lpu : node.lpus()) {
        Identifier& id = (*info.mutable_lpu_identifiers())[host_lpu.cpu_id()];
        id.set_name(host_lpu.name());
        PopulateId(id);
      }
    }
  }

  if (InfoTypeHave(req.info_types(), cpu::InfoType::SOCKET_IDENTIFIER)) {
    RETURN_IF_ERROR(topology.Gather(*host_adapter_));
    for (const CpuNumaNode& node : topology.numa_nodes()) {
      for (const CpuLpu& host_lpu : node.lpus()) {
        Identifier& id =
            (*info.mutable_socket_identifiers())[host_lpu.cpu_id()];
        id.set_name(host_lpu.socket_name());
        PopulateId(id);
      }
    }
  }

  if (InfoTypeHave(req.info_types(), cpu::InfoType::PACKAGE_IDENTIFIER)) {
    RETURN_IF_ERROR(topology.Gather(*host_adapter_));
    for (const CpuNumaNode& node : topology.numa_nodes()) {
      for (const CpuLpu& host_lpu : node.lpus()) {
        Identifier& id =
            (*info.mutable_package_identifiers())[host_lpu.cpu_id()];
        // Socket is usually the FRU/package.
        id.set_name(host_lpu.socket_name());
        PopulateId(id);
      }
    }
  }
  return resp;
}

absl::StatusOr<GetErrorsResponse> HostBackend::GetErrors(
    const GetErrorsRequest& req) {
  GetErrorsResponse resp;
  RasDaemonErrors ras;

  if (req.has_filter() && req.filter().start_timestamp().seconds() >=
                              req.filter().end_timestamp().seconds()) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "Filter start_timestamp = %d is greater or equal to end_timestamp = %d",
        req.filter().start_timestamp().seconds(),
        req.filter().end_timestamp().seconds()));
  }

  RETURN_IF_ERROR(ras.Gather(*host_adapter_));

  if (InfoTypeHave(req.event_sources(), error::EventSource::MEMORY_ERROR)) {
    for (const Error& err : ras.memory_errors()) {
      google::protobuf::Timestamp ts;
      ts.set_seconds(absl::ToUnixSeconds(err.timestamp));

      if (req.has_filter() &&
          (ts.seconds() <= req.filter().start_timestamp().seconds() ||
           ts.seconds() > req.filter().end_timestamp().seconds())) {
        continue;
      }

      if (err.label == "unknown memory") {
        continue;
      }

      error::ErrorInfo& err_info = *resp.add_errors();
      err_info.set_event_source(error::EventSource::MEMORY_ERROR);
      *err_info.mutable_event_timestamp() = ts;
      err_info.set_count(err.count);
      err_info.set_is_correctable(err.is_correctable);
      err_info.set_is_fatal(err.is_fatal);

      Identifier& error_info_identifier = *err_info.add_ids();
      error_info_identifier.set_type("dimm");
      error_info_identifier.set_name(err.label);
      PopulateId(error_info_identifier);

      if (req.report_debug_info()) {
        err_info.set_other_debug_info(err.debuginfo);
      }
    }
  }
  return resp;
}

absl::StatusOr<GetHwInfoResponse> HostBackend::GetHwInfo(
    const GetHwInfoRequest& request) {
  GetHwInfoResponse response;

  for (absl::string_view id : request.ids()) {
    ASSIGN_OR_RETURN(Identifier identifier, UnmarshalIdentifier(id));

    third_party::ocpdiag::results_pb::HardwareInfo& hardware_info =
        (*response.mutable_hw_info())[id];
    hardware_info.set_name(identifier.name());
    hardware_info.set_part_type(identifier.type());
    hardware_info.set_arena(identifier.arena());

    // Set component location.
    hardware_info.mutable_component_location()->set_odata_id(
        identifier.odata_id());
    hardware_info.mutable_component_location()->set_devpath(
        identifier.devpath());
  }
  return response;
}

absl::StatusOr<RebootResponse> HostBackend::Reboot(const RebootRequest& req) {
  if (req.type() == RebootRequest::REBOOTTYPE_SOFT) {
    RETURN_IF_ERROR(
        host_adapter_->RunCommand(kSoftRebootTimeout, {"shutdown", "-r", "now"})
            .status());
  } else if (req.type() == RebootRequest::REBOOTTYPE_HARD) {
    return absl::UnimplementedError(absl::StrFormat(
        "Reboot type %s unimplemented.", req.RebootType_Name(req.type())));
  } else {
    return absl::InternalError(absl::StrCat("Unsupported reboot type: ",
                                            req.RebootType_Name(req.type())));
  }
  return RebootResponse();
}

}  // namespace internal

}  // namespace ocpdiag::hwinterface
