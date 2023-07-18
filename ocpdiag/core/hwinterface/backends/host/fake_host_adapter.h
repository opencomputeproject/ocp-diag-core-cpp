// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

// Classes for mock/fake HostAdapter.

#ifndef OCPDIAG_CORE_HWINTERFACE_BACKENDS_HOST_FAKE_HOST_ADAPTER_H_
#define OCPDIAG_CORE_HWINTERFACE_BACKENDS_HOST_FAKE_HOST_ADAPTER_H_

#include "ocpdiag/core/hwinterface/backends/lib/fake_host_adapter.h"

namespace ocpdiag::hwinterface::internal {

// RealisticHostAdapter fakes a host adapter with 2 sockets cpu, 2 cores per
// socket, and 2 threads per core. Currently only implements CpuInfo relatated
// entries on procfs and sysfs filesystem.
class RealisticHostAdapter final : public FakeHostAdapter {
 public:
  RealisticHostAdapter(bool support_cpu_scaling = true);
};

}  // namespace ocpdiag::hwinterface::internal

#endif  // OCPDIAG_CORE_HWINTERFACE_BACKENDS_HOST_FAKE_HOST_INTERFACE_H_
