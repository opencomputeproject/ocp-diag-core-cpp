// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_HWINTERFACE_BACKENDS_LIB_MOCK_HOST_ADAPTER_H_
#define OCPDIAG_CORE_HWINTERFACE_BACKENDS_LIB_MOCK_HOST_ADAPTER_H_

#include "gmock/gmock.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "ocpdiag/core/hwinterface/backends/lib/host_adapter.h"

namespace ocpdiag::hwinterface {

// HostAdapter mock
class MockHostAdapter final : public HostAdapter {
 public:
  MockHostAdapter() {
    ON_CALL(*this, Read)
        .WillByDefault(::testing::Return(absl::StatusOr<std::string>("")));

    // Set up the default command to return the hostname, so that GetHostname()
    // works by default.
    ON_CALL(*this, RunCommand)
        .WillByDefault(::testing::Return(
            CommandResult{.stdout = "mock_hostname.prod.google.com"}));
  }
  ~MockHostAdapter() override = default;

  MOCK_METHOD(absl::StatusOr<std::string>, Read,
              (const std::filesystem::path& path), (override));
  MOCK_METHOD(absl::Status, Write,
              (const std::filesystem::path& path, absl::string_view data),
              (override));
  MOCK_METHOD(absl::StatusOr<CommandResult>, RunCommand,
              (absl::Duration timeout, const std::vector<std::string>& args),
              (override));
};

}  // namespace ocpdiag::hwinterface

#endif  // OCPDIAG_CORE_HWINTERFACE_BACKENDS_LIB_MOCK_HOST_ADAPTER_H_
