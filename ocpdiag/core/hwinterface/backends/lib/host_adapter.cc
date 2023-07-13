// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/backends/lib/host_adapter.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <string>

#include "absl/cleanup/cleanup.h"
#include "absl/log/log.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/ascii.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/time/clock.h"
#include "ecclesia/lib/apifs/apifs.h"
#include "ocpdiag/core/compat/status_macros.h"
#include "ocpdiag/core/lib/off_dut_machine_interface/remote.h"
#include "ocpdiag/core/lib/off_dut_machine_interface/remote_factory.h"

namespace ocpdiag::hwinterface {

constexpr absl::Duration kPollChildInterval = absl::Milliseconds(100);

namespace {

// Helper to read the entire contents of a file
absl::StatusOr<std::string> ReadLocalFile(FILE* file) {
  std::fseek(file, 0, SEEK_END);
  std::string out;
  out.resize(std::ftell(file));
  std::rewind(file);
  size_t read = std::fread(out.data(), sizeof(char), out.size(), file);
  if (read < out.size()) {
    return absl::InternalError("Failed to read entirety of file.");
  }
  return out;
}

}  // namespace

absl::StatusOr<LocalHostAdapter::CommandResult> LocalHostAdapter::RunCommand(
    absl::Duration timeout, const std::vector<std::string>& args) {
  if (args.empty()) {
    return absl::InvalidArgumentError("Empty args.");
  }

  LocalHostAdapter::CommandResult result;

  // Prepare the stdout and stderr temp files.
  FILE* fstdout = std::tmpfile();
  FILE* fstderr = std::tmpfile();
  if (fstdout == nullptr || fstderr == nullptr) {
    return absl::InternalError("Failed to create tmpfile file.");
  }
  auto file_cleanup = absl::MakeCleanup([fstdout, fstderr]() {
    std::fclose(fstdout);
    std::fclose(fstderr);
  });

  pid_t pid = fork();
  if (pid < 0) {
    return absl::InternalError(
        absl::StrFormat("Failed to fork the process: %s.", strerror(errno)));
  }

  // Child process, never returns.
  if (pid == 0) {
    if (dup2(fileno(fstdout), STDOUT_FILENO) == -1) {
      std::_Exit(errno);
    }
    if (dup2(fileno(fstderr), STDERR_FILENO) == -1) {
      std::_Exit(errno);
    }

    //  convert args to char* const[] required by execvp.
    std::vector<std::string> vargs(args.size());
    std::copy(args.begin(), args.end(), vargs.begin());

    std::vector<char*> pargs(args.size() + 1, nullptr);
    for (std::size_t i = 0; i < vargs.size(); ++i) {
      pargs[i] = const_cast<char*>(vargs[i].c_str());
    }
    execvp(pargs.data()[0], pargs.data());
    // If a command is not found, the child process created to execute it
    // returns 127. If a command is found but is not executable, return 126.
    std::_Exit((errno == ENOENT) ? 127 : 126);
  }

  // Below is the parent process code.
  // Wait for the child process to finish, or kill the command if it times out.
  absl::Time deadline = absl::Now() + timeout;
  while (absl::Now() < deadline) {
    int status = 0;

    pid_t w = waitpid(pid, &status, WNOHANG);

    if (w == 0) {
      absl::SleepFor(kPollChildInterval);
      continue;
    }
    if (w != pid) {
      return absl::InternalError(absl::StrCat(
          "Failed to wait for the child process.: ", strerror(errno)));
    }

    // Command finishes successfully. Populate the result.
    HostAdapter::CommandResult result;
    if (WIFEXITED(status)) {
      result.exit_code = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
      // When a command terminates on a fatal signal whose number is N, Bash
      // uses the value 128+N as the exit status.
      result.exit_code = 128 + WTERMSIG(status);
    } else {
      return absl::InternalError(absl::StrCat(
          "Unexpected state: waitpid returned when subprocess neither exited "
          "nor terminated by signal. status=",
          status));
    }

    // Copy stdout and stderr to the result.
    ASSIGN_OR_RETURN(result.stdout, ReadLocalFile(fstdout));
    ASSIGN_OR_RETURN(result.stderr, ReadLocalFile(fstderr));
    return result;
  }

  // Timeout, kill the command and return.
  kill(pid, SIGKILL);
  return absl::DeadlineExceededError(absl::StrFormat(
      "Command failed to finish in %s.", absl::FormatDuration(timeout)));

  return result;
}

absl::StatusOr<std::string> LocalHostAdapter::Read(
    const std::filesystem::path& path) {
  ecclesia::ApifsFile file(path.string());
  return file.Read();
}

absl::Status LocalHostAdapter::Write(const std::filesystem::path& path,
                                     absl::string_view data) {
  ecclesia::ApifsFile file(path.string());
  if (!file.Exists()) {
    close(creat(path.c_str(), 0777));
  }
  return file.Write(data);
}

absl::StatusOr<std::unique_ptr<RemoteHostAdapter>> RemoteHostAdapter::Create(
    absl::string_view remote_host_address) {
  remote::NodeSpec spec{
      .address = std::string{remote_host_address},
  };
  LOG(INFO) << __FUNCTION__ << ": " << remote_host_address;

  ASSIGN_OR_RETURN(std::unique_ptr<remote::ConnInterface> conn,
                   remote::NewConn(spec));
  return std::make_unique<RemoteHostAdapter>(std::move(conn));
}

absl::StatusOr<HostAdapter::CommandResult> RemoteHostAdapter::RunCommand(
    absl::Duration timeout, const std::vector<std::string>& args) {
  ASSIGN_OR_RETURN(remote::ConnInterface::CommandResult result,
                   connection_->RunCommand(
                       timeout, args, remote::ConnInterface::CommandOption()));

  return HostAdapter::CommandResult{
      .exit_code = result.exit_code,
      .stdout = result.stdout,
      .stderr = result.stderr,
  };
}

absl::StatusOr<std::string> RemoteHostAdapter::Read(
    const std::filesystem::path& path) {
  ASSIGN_OR_RETURN(absl::Cord result, connection_->ReadFile(path.string()));
  return std::string(result);
}

absl::Status RemoteHostAdapter::Write(const std::filesystem::path& path,
                                      absl::string_view data) {
  return connection_->WriteFile(path.string(), data);
}

absl::StatusOr<std::string> HostAdapter::GetHostname(
    absl::Duration timeout /*= absl::Seconds(30)*/) {
  ASSIGN_OR_RETURN(CommandResult result,
                   RunCommand(timeout, {"/bin/hostname"}));
  if (result.exit_code != EXIT_SUCCESS) {
    return absl::UnknownError(
        absl::StrFormat("Attempt to get hostname failed with exit code '%d' "
                        "and error logs: %s",
                        result.exit_code, result.stderr));
  }
  // Strip whitespace to avoid trailing newlines.
  absl::StripAsciiWhitespace(&result.stdout);
  return result.stdout;
}

absl::StatusOr<std::unique_ptr<HostAdapter>> HostAdapter::Create(
    absl::string_view address) {
  if (address.empty()) return std::make_unique<LocalHostAdapter>();
  return RemoteHostAdapter::Create(address);
}

}  // namespace ocpdiag::hwinterface
