// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/backends/lib/fake_host_adapter.h"

#include "absl/log/log.h"
#include "absl/status/status.h"
#include "absl/strings/str_join.h"

namespace ocpdiag::hwinterface {

FakeHostAdapter::~FakeHostAdapter() = default;

absl::StatusOr<HostAdapter::CommandResult> FakeHostAdapter::RunCommand(
    absl::Duration timeout, const std::vector<std::string>& args) {

  // If we were given a run function, then run that.
  if (run_command_callback_) {
    std::variant<absl::StatusCode, CommandResult> res =
        run_command_callback_(timeout, args);
    if (res.index() == 0)
      return absl::Status(std::get<absl::StatusCode>(res), "Injected Error");
    return std::get<CommandResult>(res);
  }

  // Return the hostname by default, so that GetHostname() works correctly.
  return CommandResult{
      .exit_code = EXIT_SUCCESS,
      .stdout = "fake_hostname.prod.google.com",
  };
}

absl::StatusOr<std::string> FakeHostAdapter::Read(
    const std::filesystem::path& path) {
  auto iter = fake_filesystem_.find(path);
  if (iter == fake_filesystem_.end()) {
    // Return NotFound for any file that wasn't explicitly initialized.
    return absl::NotFoundError(path.string());
  }
  FileInfo& info = iter->second;
  if (info.read_error != absl::StatusCode::kOk)
    return absl::Status(info.read_error, "Injected Read Error");
  return info.contents;
}

absl::Status FakeHostAdapter::Write(const std::filesystem::path& path,
                                    absl::string_view data) {
  FileInfo& info = fake_filesystem_[path];
  if (info.write_error != absl::StatusCode::kOk)
    return absl::Status(info.write_error, "Injected Write Error");
  info.contents = data;
  return absl::OkStatus();
}

}  // namespace ocpdiag::hwinterface
