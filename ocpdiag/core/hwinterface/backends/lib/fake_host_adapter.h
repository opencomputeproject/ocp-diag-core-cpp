// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_HWINTERFACE_BACKENDS_LIB_FAKE_HOST_ADAPTER_H_
#define OCPDIAG_CORE_HWINTERFACE_BACKENDS_LIB_FAKE_HOST_ADAPTER_H_

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "ocpdiag/core/hwinterface/backends/lib/host_adapter.h"

namespace ocpdiag::hwinterface {

// Implements a fake HostAdapter object with a simplistic in-memory filesystem.
// To customize RunCommand behavior, simply provide a callback.
class FakeHostAdapter : public HostAdapter {
 public:
  // Function signature for RunCommand.
  using RunCommandFunction =
      std::function<std::variant<absl::StatusCode, CommandResult>(
          absl::Duration, const std::vector<std::string>&)>;

  FakeHostAdapter() = default;
  ~FakeHostAdapter() override;

  // Set a custom callback to run instead of the default behavior.
  // By default, RunCommand simply returns a fake hostname.
  void SetRunCommandCallback(const RunCommandFunction& callback) {
    run_command_callback_ = callback;
  }

  // Injects an error to subsequent calls to Read() at the given path.
  void SetReadError(const std::filesystem::path& path,
                    absl::StatusCode error_code) {
    fake_filesystem_[path].read_error = error_code;
  }

  // Injects an error to subsequent calls to Write() at the given path.
  void SetWriteError(const std::filesystem::path& path,
                     absl::StatusCode error_code) {
    fake_filesystem_[path].write_error = error_code;
  }

  // Satisfies the HostAdapter interface.
  absl::StatusOr<CommandResult> RunCommand(
      absl::Duration timeout, const std::vector<std::string>& args) override;
  absl::StatusOr<std::string> Read(const std::filesystem::path& path) override;
  absl::Status Write(const std::filesystem::path& path,
                     absl::string_view data) override;

 private:
  struct FileInfo {
    std::string contents;
    absl::StatusCode read_error = absl::StatusCode::kOk;
    absl::StatusCode write_error = absl::StatusCode::kOk;
  };

  std::unordered_map<std::string, FileInfo> fake_filesystem_;
  RunCommandFunction run_command_callback_;
};

}  // namespace ocpdiag::hwinterface

#endif  // OCPDIAG_CORE_HWINTERFACE_BACKENDS_LIB_FAKE_HOST_ADAPTER_H_
