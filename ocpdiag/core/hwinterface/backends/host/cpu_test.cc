// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/backends/host/cpu.h"

#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "ocpdiag/core/hwinterface/backends/host/fake_host_adapter.h"
#include "ocpdiag/core/hwinterface/cpu.pb.h"
#include "ocpdiag/core/testing/file_utils.h"
#include "ocpdiag/core/testing/status_matchers.h"

namespace ocpdiag::hwinterface {

namespace internal {
namespace {

using ::ocpdiag::testutils::GetDataDependencyFileContents;

TEST(CpuTopologyTest, FakeHostAdapter) {
  CpuTopology topology;
  RealisticHostAdapter host_adapter;
  ASSERT_EQ(topology.Gather(host_adapter), absl::OkStatus());

  EXPECT_EQ(topology.numa_nodes().size(), 2);
  EXPECT_EQ(topology.sockets_count(), 2);
  EXPECT_EQ(topology.physical_cores_count(), 4);
  EXPECT_EQ(topology.logical_cores_count(), 8);
  EXPECT_EQ(topology.logical_cores_max_per_socket(), 4);

  int numa_node_id_expected = 0;
  for (const auto& node : topology.numa_nodes()) {
    EXPECT_EQ(node.numa_node_id(), numa_node_id_expected);
    EXPECT_EQ(node.logical_processing_unit_count(), 4)
        << "numa_node id:" << node.numa_node_id();
    EXPECT_EQ(node.physical_processing_unit_count(), 2)
        << "numa_node id:" << node.numa_node_id();
    for (const auto& lpu : node.lpus()) {
      EXPECT_EQ(lpu.socket_id(), numa_node_id_expected)
          << "lpu cpu id:" << lpu.cpu_id();
    }
    ++numa_node_id_expected;
  }
}

TEST(CpuLpuTest, FakeHostAdapter) {
  RealisticHostAdapter host_adapter;
  CpuLpu lpu1(1, 0);
  ASSERT_EQ(lpu1.Gather(host_adapter), absl::OkStatus());

  EXPECT_EQ(lpu1.name(), "/CPU0/die0/core1/thread0#processor1");
  EXPECT_EQ(lpu1.socket_name(), "CPU0");

  CpuLpu lpu7(7, 1);
  ASSERT_EQ(lpu7.Gather(host_adapter), absl::OkStatus());

  EXPECT_EQ(lpu7.name(), "/CPU1/die0/core1/thread1#processor7");
  EXPECT_EQ(lpu7.socket_name(), "CPU1");
}

TEST(CpuSignatureTest, IntelCpu) {
  CpuSignature sig;
  ASSERT_EQ(sig.ParseFromCpuInfo(GetDataDependencyFileContents(
                "ocpdiag/core/hwinterface/backends/host/"
                "testdata/cpu_signature_test/cpuinfo.intel.tsv")),
            absl::OkStatus());
  EXPECT_EQ(sig.vendor(), cpu::Vendor::VENDOR_INTEL);
  EXPECT_EQ(sig.family(), 6);
  EXPECT_EQ(sig.model(), 85);
  EXPECT_EQ(sig.stepping(), 3);
  EXPECT_EQ(sig.microcode(), 2348811424);
}

TEST(CpuSignatureTest, AmdCpu) {
  CpuSignature sig;
  EXPECT_EQ(sig.ParseFromCpuInfo(GetDataDependencyFileContents(
                "ocpdiag/core/hwinterface/backends/host/"
                "testdata/cpu_signature_test/cpuinfo.amd.tsv")),
            absl::OkStatus());
  EXPECT_EQ(sig.vendor(), cpu::Vendor::VENDOR_AMD);
  EXPECT_EQ(sig.family(), 23);
  EXPECT_EQ(sig.model(), 1);
  EXPECT_EQ(sig.stepping(), 1);
  EXPECT_EQ(sig.microcode(), 134222108);
}

TEST(CpuSignatureTest, ArmCpu) {
  CpuSignature sig;
  EXPECT_EQ(sig.ParseFromCpuInfo(GetDataDependencyFileContents(
                "ocpdiag/core/hwinterface/backends/host/"
                "testdata/cpu_signature_test/cpuinfo.arm.tsv")),
            absl::OkStatus());
  EXPECT_EQ(sig.vendor(), cpu::Vendor::VENDOR_ARM);
  EXPECT_EQ(sig.family(), 0);
  EXPECT_EQ(sig.model(), 0);
  EXPECT_EQ(sig.stepping(), 0);
  EXPECT_EQ(sig.microcode(), 0);
}

TEST(CpuSignatureTest, UnknownCpu) {
  CpuSignature sig;
  EXPECT_EQ(sig.ParseFromCpuInfo(GetDataDependencyFileContents(
                "ocpdiag/core/hwinterface/backends/host/"
                "testdata/cpu_signature_test/cpuinfo.unknown.tsv")),
            absl::OkStatus());
  EXPECT_EQ(sig.vendor(), cpu::Vendor::VENDOR_UNKNOWN);
  EXPECT_EQ(sig.family(), 23);
  EXPECT_EQ(sig.model(), 1);
  EXPECT_EQ(sig.stepping(), 0);
  EXPECT_EQ(sig.microcode(), 0);
}

TEST(CpuFrequencyTest, FakeHostAdapter) {
  CpuTopology topology;
  RealisticHostAdapter host;

  ASSERT_EQ(topology.Gather(host), absl::OkStatus());

  for (const auto& node : topology.numa_nodes()) {
    for (const auto& lpu : node.lpus()) {
      CpuFrequency freq(lpu.cpu_id());
      EXPECT_EQ(freq.Gather(host), absl::OkStatus())
          << "cpu id: " << lpu.cpu_id();
      EXPECT_EQ(freq.design_freq(), 2000127) << "cpu id: " << lpu.cpu_id();
      EXPECT_EQ(freq.cur_freq(), 1000000 + lpu.cpu_id())
          << "cpu id: " << lpu.cpu_id();
      EXPECT_EQ(freq.max_freq(), 2001000) << "cpu id: " << lpu.cpu_id();
    }
  }
}

TEST(CpuFrequencyTest, SystemNotDiscloseCpuScalingInfo) {
  CpuTopology topology;
  RealisticHostAdapter host(/*support_cpu_scaling=*/false);

  ASSERT_OK(topology.Gather(host));

  for (const auto& node : topology.numa_nodes()) {
    for (const auto& lpu : node.lpus()) {
      CpuFrequency freq(lpu.cpu_id());
      ASSERT_OK(freq.Gather(host)) << "cpu id: " << lpu.cpu_id();
      EXPECT_EQ(freq.design_freq(), 2000127) << "cpu id: " << lpu.cpu_id();
      EXPECT_EQ(freq.cur_freq(), freq.design_freq());
      EXPECT_EQ(freq.max_freq(), freq.design_freq());
    }
  }
}

TEST(CpuThrottleInfoTest, FakeHostAdapter) {
  CpuTopology topology;
  RealisticHostAdapter host;

  ASSERT_EQ(topology.Gather(host), absl::OkStatus());

  for (const auto& node : topology.numa_nodes()) {
    for (const auto& lpu : node.lpus()) {
      CpuThrottleInfo throttle(lpu.cpu_id());
      EXPECT_EQ(throttle.Gather(host), absl::OkStatus())
          << "cpu id: " << lpu.cpu_id();
      EXPECT_EQ(throttle.core_throttle_count(), lpu.cpu_id())
          << "cpu id: " << lpu.cpu_id();
    }
  }
}

}  // namespace
}  // namespace internal

}  // namespace ocpdiag::hwinterface
