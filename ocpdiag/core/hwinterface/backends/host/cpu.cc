// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/backends/host/cpu.h"

#include <stdint.h>

#include <algorithm>
#include <filesystem>
#include <iterator>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/match.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "ocpdiag/core/compat/status_macros.h"
#include "ocpdiag/core/hwinterface/backends/lib/utils.h"
#include "ocpdiag/core/hwinterface/cpu.pb.h"
#include "re2/re2.h"

namespace ocpdiag::hwinterface {
namespace {

bool SimpleHexAtoi(absl::string_view hex, uint64_t& num) {
  if (absl::numbers_internal::safe_strtoi_base(hex, &num, 16)) {
    return true;
  }
  return false;
}

}  // namespace

namespace internal {

// Parses the content of file `path` into an range list.
// Example:
//     ParseRangeListFile("1,3-5,7,9-10") return {1,3,4,5,7,9,10}
absl::StatusOr<std::vector<int>> ParseRangeListFile(
    HostAdapter& host, const std::filesystem::path& path) {
  ASSIGN_OR_RETURN(std::string list_range, host.Read(path));

  absl::StatusOr<std::vector<int>> nums = ParseRangeList(list_range);
  if (!nums.ok()) {
    return absl::InternalError(absl::StrCat(
        "File parsed failed: ", path.string(), " : ", nums.status().message()));
  }
  return nums;
}

// Parses the content of file `path` into an integer.
absl::StatusOr<int> ParseIntegerFile(HostAdapter& host,
                                     const std::filesystem::path& path) {
  int result = 0;
  ASSIGN_OR_RETURN(std::string integer_str, host.Read(path));
  if (!absl::SimpleAtoi(integer_str, &result)) {
    return absl::InternalError(
        absl::StrCat("File parsed failed: ", path.string()));
  }

  return result;
}

void CpuLpu::NormalizeCpuLpu(absl::flat_hash_map<int, int>& socket_ids_map,
                             absl::flat_hash_map<int, int>& core_ids_map) {
  socket_id_ = socket_ids_map[socket_id_];
  core_id_ = core_ids_map[core_id_];
}

absl::Status CpuLpu::DoRealGather(HostAdapter& host) {
  std::filesystem::path sysfs_cpu_path(
      absl::StrFormat("/sys/devices/system/cpu/cpu%d", cpu_id_));

  ASSIGN_OR_RETURN(
      socket_id_,
      ParseIntegerFile(host, sysfs_cpu_path / "topology/physical_package_id"));
  ASSIGN_OR_RETURN(die_id_,
                   ParseIntegerFile(host, sysfs_cpu_path / "topology/die_id"));
  ASSIGN_OR_RETURN(core_id_,
                   ParseIntegerFile(host, sysfs_cpu_path / "topology/core_id"));

  ASSIGN_OR_RETURN(std::vector<int> thread_siblings,
                   ParseRangeListFile(
                       host, sysfs_cpu_path / "topology/thread_siblings_list"));
  auto it = std::find(thread_siblings.begin(), thread_siblings.end(), cpu_id_);
  if (it == thread_siblings.end()) {
    return absl::InternalError(
        absl::StrCat("Cpu id: ", cpu_id_, " not in it's thread_siblings_list"));
  }
  thread_id_ = std::distance(thread_siblings.begin(), it);

  return absl::OkStatus();
}

absl::Status CpuLpu::Gather(HostAdapter& host) {
  if (result_.has_value()) {
    return result_.value();
  }
  result_ = DoRealGather(host);
  return result_.value();
}

void CpuNumaNode::NormalizeNumaNode(
    absl::flat_hash_map<int, int>& socket_ids_map,
    absl::flat_hash_map<int, int>& core_ids_map) {
  for (CpuLpu& lpu : lpus_) {
    lpu.NormalizeCpuLpu(socket_ids_map, core_ids_map);
  }
}

absl::Status CpuNumaNode::DoRealGather(HostAdapter& host) {
  std::filesystem::path cpulist_path(absl::StrFormat(
      "/sys/devices/system/node/node%d/cpulist", numa_node_id_));

  ASSIGN_OR_RETURN(std::vector<int> cpu_ids,
                   ParseRangeListFile(host, cpulist_path));

  for (int cpu_id : cpu_ids) {
    CpuLpu lpu(cpu_id, numa_node_id_);
    RETURN_IF_ERROR(lpu.Gather(host));
    physical_processing_unit_count_ += (lpu.thread_id() == 0) ? 1 : 0;
    lpus_.push_back(std::move(lpu));
  }

  return absl::OkStatus();
}

absl::Status CpuNumaNode::Gather(HostAdapter& host) {
  if (result_.has_value()) {
    return result_.value();
  }
  result_ = DoRealGather(host);
  return result_.value();
}

void CpuTopology::NormalizeTopology() {
  absl::flat_hash_map<int, int> socket_ids_map;
  int socket_num = 0;

  for (CpuNumaNode& node : numa_nodes_) {
    absl::flat_hash_map<int, int> core_ids_map;
    int core_num = 0;

    for (const CpuLpu& lpu : node.lpus()) {
      auto socket_it = socket_ids_map.find(lpu.socket_id());
      if (socket_it == socket_ids_map.end()) {
        socket_ids_map.insert({lpu.socket_id(), socket_num++});
      }

      auto core_it = core_ids_map.find(lpu.core_id());
      if (core_it == core_ids_map.end()) {
        core_ids_map.insert({lpu.core_id(), core_num++});
      }
    }
    node.NormalizeNumaNode(socket_ids_map, core_ids_map);
  }
}

absl::Status CpuTopology::DoRealGather(HostAdapter& host) {
  std::filesystem::path node_online_path("/sys/devices/system/node/online");

  ASSIGN_OR_RETURN(std::vector<int> numa_node_ids,
                   ParseRangeListFile(host, node_online_path));

  for (int numa_id : numa_node_ids) {
    CpuNumaNode numa_node(numa_id);
    RETURN_IF_ERROR(numa_node.Gather(host));
    numa_nodes_.push_back(std::move(numa_node));
  }

  NormalizeTopology();

  for (const CpuNumaNode& node : numa_nodes_) {
    for (const CpuLpu& lpu : node.lpus()) {
      socket_lpus_map_[lpu.socket_id()].push_back(lpu.cpu_id());
    }
  }

  return absl::OkStatus();
}

absl::Status CpuTopology::Gather(HostAdapter& host) {
  if (result_.has_value()) {
    return result_.value();
  }
  result_ = DoRealGather(host);
  return result_.value();
}

absl::Status CpuSignature::DoRealGather(HostAdapter& host) {
  ASSIGN_OR_RETURN(std::string cpuinfo, host.Read({"/proc/cpuinfo"}));
  return ParseFromCpuInfo(cpuinfo);
}

absl::Status CpuSignature::Gather(HostAdapter& host) {
  if (result_.has_value()) {
    return result_.value();
  }
  result_ = DoRealGather(host);
  return result_.value();
}

absl::Status CpuSignature::ParseFromCpuInfo(absl::string_view cpuinfo) {
  std::string vendor_id;

  if (RE2::PartialMatch(cpuinfo, "vendor_id\t: (\\w+)\n", &vendor_id)) {
    // x86 cpus
    if (vendor_id == "GenuineIntel") {
      vendor_ = cpu::Vendor::VENDOR_INTEL;
    } else if (vendor_id == "AuthenticAMD") {
      vendor_ = cpu::Vendor::VENDOR_AMD;
    } else {
      vendor_ = cpu::Vendor::VENDOR_UNKNOWN;
    }

    RE2::PartialMatch(cpuinfo, "cpu family\t: (\\d+)\n", &family_);
    RE2::PartialMatch(cpuinfo, "model\t\t: (\\d+)\n", &model_);
    RE2::PartialMatch(cpuinfo, "stepping\t: (\\d+)\n", &stepping_);

    std::string microcode;
    if (RE2::PartialMatch(cpuinfo, "microcode\t: 0x([[:xdigit:]]+)\n",
                          &microcode)) {
      if (!SimpleHexAtoi(microcode, microcode_)) {
        return absl::InternalError(absl::StrCat(
            "microcode in /proc/cpuinfo is not a hex. ", microcode));
      }
    }
  } else if (absl::StrContains(cpuinfo, "BogoMIPS")) {
    std::string implementer;
    uint64_t implementer_code = 0;

    if (!RE2::PartialMatch(cpuinfo, "CPU implementer\t: 0x([[:xdigit:]]+)\n",
                           &implementer) ||
        !SimpleHexAtoi(implementer, implementer_code)) {
      return absl::InternalError(absl::StrCat(
          "implementer in /proc/cpuinfo is not a hex. ", implementer));
    }

    switch (implementer_code) {
      case 'A':
        vendor_ = cpu::Vendor::VENDOR_ARM;
        break;
      case 'P':
        vendor_ = cpu::Vendor::VENDOR_APM;
        break;
      case 'Q':
        vendor_ = cpu::Vendor::VENDOR_QUALCOMM;
        break;
      default:
        vendor_ = cpu::Vendor::VENDOR_UNKNOWN;
        break;
    }
  } else {
    vendor_ = cpu::Vendor::VENDOR_UNKNOWN;
  }
  return absl::OkStatus();
}

absl::Status CpuFrequency::Gather(HostAdapter& host) {
  ASSIGN_OR_RETURN(std::string cpuinfo, host.Read({"/proc/cpuinfo"}));
  std::filesystem::path sysfs_cpu_path(
      absl::StrFormat("/sys/devices/system/cpu/cpu%d", cpu_id_));

  ASSIGN_OR_RETURN(design_freq_,
                   ParseIntegerFile(host, sysfs_cpu_path / "tsc_freq_khz"));
  absl::StatusOr<int> cur_freq =
      ParseIntegerFile(host, sysfs_cpu_path / "cpufreq/scaling_cur_freq");

  if (cur_freq.ok()) {
    cur_freq_ = *cur_freq;
    ASSIGN_OR_RETURN(
        max_freq_,
        ParseIntegerFile(host, sysfs_cpu_path / "cpufreq/scaling_max_freq"));
  } else if (cur_freq.status().code() == absl::StatusCode::kNotFound) {
    // System did not disclose cpu scaling.
    cur_freq_ = design_freq_;
    max_freq_ = design_freq_;
  } else {
    return cur_freq.status();
  }

  return absl::OkStatus();
}

absl::Status CpuThrottleInfo::Gather(HostAdapter& host) {
  std::filesystem::path sysfs_cpu_path(
      absl::StrFormat("/sys/devices/system/cpu/cpu%d", cpu_id_));

  ASSIGN_OR_RETURN(
      core_throttle_count_,
      ParseIntegerFile(
          host, sysfs_cpu_path / "thermal_throttle/core_throttle_count"));

  return absl::OkStatus();
}

}  // namespace internal

}  // namespace ocpdiag::hwinterface
