// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_HWINTERFACE_BACKENDS_LIB_HOST_ADAPTER_H_
#define OCPDIAG_CORE_HWINTERFACE_BACKENDS_LIB_HOST_ADAPTER_H_

#include <filesystem>

#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "ocpdiag/core/lib/off_dut_machine_interface/remote.h"

namespace ocpdiag::hwinterface {

// HostAdapter helps program to manipulate  localhost / remote host,
// and increases testability by faking or mocking the adapter.
class HostAdapter {
 public:
  // If the address is empty, creates a LocalHostAdapter, otherwise creates a
  // RemoteHostAdapter.
  static absl::StatusOr<std::unique_ptr<HostAdapter>> Create(
      absl::string_view address);

  virtual ~HostAdapter() = default;

  // The exit code and the command's output to stdout and stderr.
  struct CommandResult {
    // The exit code is the value returned by the waitpid system call, like
    // bash.
    // If a command is not found, the child process returns a code of 127. If
    // a command is found but is not executable, the return code is 126. When
    // a command terminates on a fatal signal whose number is N, return the
    // value 128+N as the exit status.
    int exit_code;
    std::string stdout;
    std::string stderr;
  };

  // Run command `args` with `timeout`.
  // subprocess will be killed after `tiemout`.
  virtual absl::StatusOr<CommandResult> RunCommand(
      absl::Duration timeout, const std::vector<std::string>& args) = 0;

  // Read file content form `path`
  virtual absl::StatusOr<std::string> Read(
      const std::filesystem::path& path) = 0;

  // Writes data to file `path`.
  // If file is exists, truncates and rewrites it.
  // If file does not exist, creates and writes it.
  virtual absl::Status Write(const std::filesystem::path& path,
                             absl::string_view data) = 0;

  // Gets the hostname from the host
  absl::StatusOr<std::string> GetHostname(
      absl::Duration timeout = absl::Seconds(30));
};

// LocalHostAdapter is for local host
class LocalHostAdapter final : public HostAdapter {
 public:
  absl::StatusOr<CommandResult> RunCommand(
      absl::Duration timeout, const std::vector<std::string>& args) override;

  absl::StatusOr<std::string> Read(const std::filesystem::path& path) override;

  absl::Status Write(const std::filesystem::path& path,
                     absl::string_view data) override;
};

// RemoteHostAdapter is adapter for off dut machine interface.
class RemoteHostAdapter final : public HostAdapter {
 public:
  explicit RemoteHostAdapter(std::unique_ptr<remote::ConnInterface> connection)
      : connection_(std::move(connection)) {}

  static absl::StatusOr<std::unique_ptr<RemoteHostAdapter>> Create(
      absl::string_view remote_host_address);

  absl::StatusOr<CommandResult> RunCommand(
      absl::Duration timeout, const std::vector<std::string>& args) override;

  absl::StatusOr<std::string> Read(const std::filesystem::path& path) override;

  absl::Status Write(const std::filesystem::path& path,
                     absl::string_view data) override;

 private:
  // Machine node connection.
  std::unique_ptr<remote::ConnInterface> connection_;
};

}  // namespace ocpdiag::hwinterface

#endif  // OCPDIAG_CORE_HWINTERFACE_BACKENDS_LIB_HOST_INTERFACE_H_
