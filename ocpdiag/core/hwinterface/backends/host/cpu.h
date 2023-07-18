// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_HWINTERFACE_BACKENDS_HOST_CPU_H_
#define OCPDIAG_CORE_HWINTERFACE_BACKENDS_HOST_CPU_H_

#include <stdint.h>

#include <algorithm>
#include <numeric>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "ocpdiag/core/hwinterface/backends/lib/host_adapter.h"
#include "ocpdiag/core/hwinterface/cpu.pb.h"

namespace ocpdiag::hwinterface {

namespace internal {

class CpuLpu {
 public:
  CpuLpu(int cpu_id, int numa_node_id)
      : cpu_id_(cpu_id), numa_node_id_(numa_node_id) {}

  // Gathers CPU logic proccessing unit infomation from procfs and sysfs by
  // `host` adapter. The file system will be read only the first time, and the
  // cache will be used afterwards.
  absl::Status Gather(HostAdapter& host);

  // unique id of cpu
  int cpu_id() const { return cpu_id_; }
  int socket_id() const { return socket_id_; }
  int die_id() const { return die_id_; }
  int core_id() const { return core_id_; }
  int thread_id() const { return thread_id_; }
  int numa_node_id() const { return numa_node_id_; }

  void NormalizeCpuLpu(absl::flat_hash_map<int, int>& socket_ids_map,
                       absl::flat_hash_map<int, int>& core_ids_map);

  // Returns a unique and meaningful name.
  std::string name() const {
    return absl::StrFormat("/CPU%d/die%d/core%d/thread%d#processor%d",
                           socket_id_, die_id_, core_id_, thread_id_, cpu_id());
  }

  std::string socket_name() const {
    return absl::StrFormat("CPU%d", socket_id_);
  }

 private:
  absl::Status DoRealGather(HostAdapter& host);

  int cpu_id_;
  int socket_id_ = 0;
  int die_id_ = 0;
  int core_id_ = 0;
  int thread_id_ = 0;
  int numa_node_id_ = 0;
  // Caches `result_` after Gather() was called.
  std::optional<absl::Status> result_ = std::nullopt;
};

class CpuNumaNode {
 public:
  CpuNumaNode(int numa_node_id) : numa_node_id_(numa_node_id) {}

  // Gathers CPU numa node infomation from procfs and sysfs by
  // `host` adapter. The file system will be read only the first time, and the
  // cache will be used afterwards.
  absl::Status Gather(HostAdapter& host);

  int numa_node_id() const { return numa_node_id_; }
  int logical_processing_unit_count() const { return lpus_.size(); }
  int physical_processing_unit_count() const {
    return physical_processing_unit_count_;
  }

  const std::vector<CpuLpu>& lpus() const { return lpus_; }
  void NormalizeNumaNode(absl::flat_hash_map<int, int>& socket_ids_map,
                         absl::flat_hash_map<int, int>& core_ids_map);

 private:
  absl::Status DoRealGather(HostAdapter& host);

  int numa_node_id_;
  int physical_processing_unit_count_ = 0;
  // Caches `result_` after Gather() was called.
  std::optional<absl::Status> result_ = std::nullopt;

  std::vector<CpuLpu> lpus_;
};

class CpuTopology {
 public:
  // Gathers CPU topology infomation from procfs and sysfs by
  // `host` adapter. The file system will be read only the first time, and the
  // cache will be used afterwards.
  absl::Status Gather(HostAdapter& host);

  const std::vector<CpuNumaNode>& numa_nodes() const { return numa_nodes_; }

  int physical_cores_count() const {
    return std::accumulate(numa_nodes_.begin(), numa_nodes_.end(), 0,
                           [](int acc, const CpuNumaNode& c) {
                             return c.physical_processing_unit_count() + acc;
                           });
  }

  int logical_cores_count() const {
    return std::accumulate(numa_nodes_.begin(), numa_nodes_.end(), 0,
                           [](int acc, const CpuNumaNode& c) {
                             return c.logical_processing_unit_count() + acc;
                           });
  }

  int logical_cores_max_per_socket() const {
    int max_per_socket = 0;
    for (const auto& [_, lpus] : socket_lpus_map_) {
      max_per_socket = std::max(max_per_socket, static_cast<int>(lpus.size()));
    }
    return max_per_socket;
  }

  int sockets_count() const { return socket_lpus_map_.size(); }

 private:
  absl::Status DoRealGather(HostAdapter& host);
  void NormalizeTopology();

  // Caches `result_` after Gather() was called.
  std::optional<absl::Status> result_ = std::nullopt;

  std::vector<CpuNumaNode> numa_nodes_;
  // Mapping socket_id and lpu_ids list.
  absl::flat_hash_map<int, std::vector<int>> socket_lpus_map_;
};

class CpuSignature {
 public:
  // Gathers CPU signature from procfs by `host` adapter. The file system will
  // be read only the first time, and the cache will be used afterwards.
  absl::Status Gather(HostAdapter& host);

  // Parses cpu signature from /proc/cpuinfo.
  absl::Status ParseFromCpuInfo(absl::string_view cpuinfo);

  cpu::Vendor vendor() const { return vendor_; }
  int family() const { return family_; }
  int model() const { return model_; }
  int stepping() const { return stepping_; }
  uint64_t microcode() const { return microcode_; }

 private:
  absl::Status DoRealGather(HostAdapter& host);

  // Signature
  cpu::Vendor vendor_ = cpu::Vendor::VENDOR_UNKNOWN;
  int family_ = 0;
  int model_ = 0;
  int stepping_ = 0;
  uint64_t microcode_ = 0;
  // Caches `result_` after Gather() was called.
  std::optional<absl::Status> result_ = std::nullopt;
};

class CpuFrequency {
 public:
  CpuFrequency(int cpu_id) : cpu_id_(cpu_id) {}

  // Gathers CPU frequencies from sysfs by `host` adapter. Data will be upated
  // on each call.
  absl::Status Gather(HostAdapter& host);

  int cpu_id() const { return cpu_id_; }
  int design_freq() const { return design_freq_; }
  int cur_freq() const { return cur_freq_; }
  int max_freq() const { return max_freq_; }

 private:
  int cpu_id_;
  int design_freq_ = 0;
  int cur_freq_ = 0;
  int max_freq_ = 0;
};

class CpuThrottleInfo {
 public:
  CpuThrottleInfo(int cpu_id) : cpu_id_(cpu_id) {}

  // Gathers CPU Throttle count from sysfs by `host` adapter. Data will be
  // upated on each call.
  absl::Status Gather(HostAdapter& host);

  int cpu_id() const { return cpu_id_; }
  int core_throttle_count() const { return core_throttle_count_; }

 private:
  int cpu_id_;
  int core_throttle_count_ = 0;
};

}  // namespace internal

}  // namespace ocpdiag::hwinterface

#endif  // OCPDIAG_CORE_HWINTERFACE_BACKENDS_HOST_CPU_H_
