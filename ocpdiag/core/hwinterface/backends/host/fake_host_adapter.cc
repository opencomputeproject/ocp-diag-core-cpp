// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/backends/host/fake_host_adapter.h"

#include <string>
#include <tuple>
#include <vector>

#include "absl/log/check.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "absl/strings/string_view.h"
#include "ocpdiag/core/testing/file_utils.h"

namespace ocpdiag::hwinterface {

namespace internal {
namespace {
// Returns the contents in `filename` under
// "ocpdiag/core/hwinterface/backends/host/testdata/fake_host_adapter/"
std::string GetTestDataContents(absl::string_view filename) {
  const std::string kTestCaseDirPath =
      "ocpdiag/core/hwinterface/backends/host/testdata/"
      "fake_host_adapter";
  return testutils::GetDataDependencyFileContents(
      absl::StrJoin(std::make_tuple(kTestCaseDirPath, filename), "/"));
}
}  // namespace

RealisticHostAdapter::RealisticHostAdapter(bool support_cpu_scaling) {
  // CpuInfo configs
  const int kSocketCount = 2;
  const int kThreadsPerCore = 2;
  const int kCoresPerSocket = 2;
  const int kCpuTotalCount = kSocketCount * kThreadsPerCore * kCoresPerSocket;

  CHECK_OK(Write("/proc/cpuinfo", GetTestDataContents("cpuinfo.tsv")));
  CHECK_OK(Write("/sys/devices/system/node/online", "0-1"));
  CHECK_OK(Write("/sys/devices/system/node/node0/cpulist", "0-1,4-5"));
  CHECK_OK(Write("/sys/devices/system/node/node1/cpulist", "2-3,6-7"));

  // /sys/devices/system/cpu/cpu*
  for (int cpu_id = 0; cpu_id < kCpuTotalCount; cpu_id++) {
    std::filesystem::path sysfs_cpu_path(
        absl::StrFormat("/sys/devices/system/cpu/cpu%d", cpu_id));
    CHECK_OK(Write((sysfs_cpu_path / "tsc_freq_khz").string(), "2000127"));

    // /sys/devices/system/cpu/cpu*/topology
    CHECK_OK(Write((sysfs_cpu_path / "topology/physical_package_id").string(),
                   absl::StrCat(static_cast<int>(cpu_id / kCoresPerSocket) %
                                kSocketCount)));
    CHECK_OK(Write((sysfs_cpu_path / "topology/core_id").string(),
                   absl::StrCat(cpu_id % kCoresPerSocket)));
    CHECK_OK(Write((sysfs_cpu_path / "topology/die_id").string(), "0"));

    std::vector<int> sliblings;
    sliblings.reserve(kThreadsPerCore);
    int first_cpu_id = cpu_id % (kSocketCount * kCoresPerSocket);
    for (int i = 0; i < kThreadsPerCore; ++i) {
      sliblings.push_back(first_cpu_id + i * (kSocketCount * kCoresPerSocket));
    }
    CHECK_OK(Write((sysfs_cpu_path / "topology/thread_siblings_list").string(),
                   absl::StrJoin(sliblings, ",")));

    if (support_cpu_scaling) {
      // /sys/devices/system/cpu/cpu*/cpufreq
      CHECK_OK(Write((sysfs_cpu_path / "cpufreq/scaling_max_freq").string(),
                     "2001000"));
      CHECK_OK(Write((sysfs_cpu_path / "cpufreq/scaling_cur_freq").string(),
                     absl::StrCat(1000000 + cpu_id)));
    }

    CHECK_OK(Write(
        (sysfs_cpu_path / "thermal_throttle/core_throttle_count").string(),
        absl::StrCat(cpu_id)));
  }
}

}  // namespace internal

}  // namespace ocpdiag::hwinterface
