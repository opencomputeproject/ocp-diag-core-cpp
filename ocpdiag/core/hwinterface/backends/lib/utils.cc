// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/backends/lib/utils.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "absl/log/log.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/match.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "ocpdiag/core/compat/status_macros.h"
#include "re2/re2.h"

namespace ocpdiag::hwinterface {
namespace internal {

constexpr absl::Duration kRunCommandTimeout = absl::Seconds(10);

absl::StatusOr<std::vector<int>> ParseRangeList(absl::string_view str) {
  std::vector<int> result;
  int last = -1;

  absl::Status error_status =
      absl::InvalidArgumentError(absl::StrCat("Parse stirng fialed: ", str));

  int from, to = 0;
  for (absl::string_view s : absl::StrSplit(str, ',')) {
    std::vector<absl::string_view> ranges = absl::StrSplit(s, '-');
    switch (ranges.size()) {
      case 1:
        if (!absl::SimpleAtoi(ranges[0], &from) || from <= last) {
          return error_status;
        }
        result.push_back(from);
        last = from;
        break;
      case 2:
        if (!absl::SimpleAtoi(ranges[0], &from) || from <= last ||
            !absl::SimpleAtoi(ranges[1], &to) || to < from) {
          return error_status;
        }
        for (int i = from; i <= to; ++i) {
          result.push_back(i);
        }
        last = to;
        break;
      default:
        return error_status;
    }
  }
  return result;
}

namespace {

absl::string_view NullSafeHost(absl::string_view host) {
  return host.data() == nullptr ? "(null)" : host;
}

bool NeedsBrackets(absl::string_view host) {
  return !host.empty() && host[0] != '[' && absl::StrContains(host, ':');
}

}  // namespace

std::string HostPortString(absl::string_view host, uint16_t port) {
  if (NeedsBrackets(host)) {
    return absl::StrCat("[", host, "]:", port);
  }
  return absl::StrCat(NullSafeHost(host), ":", port);
}

std::string GrpcHostPortString(absl::string_view host, uint16_t port) {
  static const RE2 kIpv4Regex = RE2(R"([0-9]+.[0-9]+.[0-9]+.[0-9]+)");
  // IPv4 & IPv6
  if (RE2::FullMatch(host, kIpv4Regex) || absl::StrContains(host, ':')) {
    return HostPortString(host, port);
  }
  return absl::StrCat("dns:///", NullSafeHost(host), ":", port);
}
absl::StatusOr<NodeInfo> FetchTargetNodeInfo(
    absl::string_view target_hostname,
    std::optional<absl::string_view> node_entity_path) {
  return NodeInfo{.node_type = NodeType::kNodeTypeCn};
}

static constexpr absl::Duration kProcessRestartRetryInterval = absl::Seconds(1);

absl::StatusOr<int> GetPidOfProcess(absl::string_view process_name,
                                    ocpdiag::hwinterface::HostAdapter& host) {
  ASSIGN_OR_RETURN(
      ocpdiag::hwinterface::HostAdapter::CommandResult check_result,
      host.RunCommand(kRunCommandTimeout,
                      {"/bin/pidof", "-s", std::string(process_name)}));
  // pidof returns 1 as exit code if no process found
  if (check_result.exit_code == 1) {
    return absl::NotFoundError(
        absl::StrFormat("Process %s is not running", process_name));
  } else if (check_result.exit_code > 1) {
    return absl::InternalError(
        absl::StrFormat("pidof: exit_code=%d", check_result.exit_code));
  }

  int pid;
  if (absl::SimpleAtoi(check_result.stdout, &pid)) {
    return pid;
  }
  return absl::InternalError(
      absl::StrFormat("pidof: invalid output=%s", check_result.stdout));
}

absl::Status RestartProcess(absl::string_view process_name,
                            ocpdiag::hwinterface::HostAdapter& host,
                            absl::Duration process_ready_wait) {
  LOG(INFO) << "Restarting process: " << process_name;
  // Get the pid of current process
  ASSIGN_OR_RETURN(int old_pid, GetPidOfProcess(process_name, host));
  LOG(INFO) << "Process: " << process_name << " current pid: " << old_pid;

  // Restart process
  ASSIGN_OR_RETURN(
      ocpdiag::hwinterface::HostAdapter::CommandResult result,
      host.RunCommand(kRunCommandTimeout,
                      {"/usr/sbin/service", "taskd", "restart-service",
                       std::string(process_name)}));
  if (result.exit_code != 0) {
    return absl::InternalError(
        absl::StrFormat("service taskd restart-service %s: exit_code=%d",
                        process_name, result.exit_code));
  }

  // Wait for the PID of process to change
  absl::Time start_time = absl::Now();
  while (absl::Now() - start_time < process_ready_wait) {
    absl::SleepFor(kProcessRestartRetryInterval);
    auto result = GetPidOfProcess(process_name, host);
    if (result.ok()) {
      int new_pid = result.value();
      if (new_pid != old_pid) {
        LOG(INFO) << "Process: " << process_name << " new pid: " << new_pid;
        return absl::OkStatus();
      }
    }
  }
  // Timeout waiting for process to restart
  return absl::InternalError(
      absl::StrFormat("Timed out waiting for %s to restart", process_name));
}

}  // namespace internal
}  // namespace ocpdiag::hwinterface
