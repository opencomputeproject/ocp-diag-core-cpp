// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/backends/host/error.h"

#include <cstdio>
#include <string>
#include <utility>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "ocpdiag/core/compat/status_macros.h"
#include "re2/re2.h"

namespace ocpdiag::hwinterface::internal {
namespace {

// Time format in `ras-mc-ctl --errors`.
constexpr char kTimeFormat[] = "%E4Y-%m-%d %H:%M:%S %z";
// RunCommand timeout second.
const int kRunCommandTimeoutSecond = 30;

enum class ParsingStep {
  MC_TITLE,
  MC_RECORD,
  FINISH,
};

inline absl::Status ParseErrorStatus(absl::string_view step,
                                     absl::string_view msg,
                                     absl::string_view line) {
  return absl::InternalError(absl::StrFormat(
      "`ras-mc-ctl --errors`: parsed failed, step: %s, msg:[%s], line [%s]",
      step, msg, line));
}

// Parses the memory controller events title.
absl::StatusOr<ParsingStep> ParseMcTitle(absl::string_view line) {
  if (line == "Memory controller events:") {
    return ParsingStep::MC_RECORD;
  }
  if (line == "No Memory errors.") {
    return ParsingStep::FINISH;
  }
  return ParseErrorStatus("MC_TITLE", "malform", line);
}

// Parses the memory controller events.
absl::StatusOr<ParsingStep> ParseMcRecord(absl::string_view line,
                                          std::vector<Error>& memory_errors) {
  if (line.empty()) {
    return ParsingStep::FINISH;
  }

  static const LazyRE2 KMcRegexp = {
      R"(^[0-9]+ ([0-9-]+ [0-9:]+ [0-9+]+) ([0-9]+) (\w+) error\(s\): .+ at (.+) location: .+$)"};

  Error memory_error;

  std::string temp_ts;
  std::string temp_err_type;

  if (!RE2::FullMatch(line, *KMcRegexp, &temp_ts, &memory_error.count,
                      &temp_err_type, &memory_error.label)) {
    return ParseErrorStatus("MC_RECORD", "malform", line);
  }

  std::string ts_parse_err;
  if (!absl::ParseTime(kTimeFormat, temp_ts, &memory_error.timestamp,
                       &ts_parse_err)) {
    return ParseErrorStatus("MC_RECORD",
                            absl::StrCat("timestamp: ", ts_parse_err), line);
  }

  if (temp_err_type == "Corrected") {
    memory_error.is_correctable = true;
    memory_error.is_fatal = false;
  } else if (temp_err_type == "Uncorrected") {
    memory_error.is_correctable = false;
    memory_error.is_fatal = false;
  } else if (temp_err_type == "Fatal") {
    memory_error.is_correctable = false;
    memory_error.is_fatal = true;
  } else if (temp_err_type == "Info") {
    return ParsingStep::MC_RECORD;
  } else {
    return ParseErrorStatus("MC_RECORD", "Unknow error type", line);
  }

  memory_error.debuginfo = line;
  memory_errors.push_back(std::move(memory_error));
  return ParsingStep::MC_RECORD;
}

}  // namespace

absl::Status RasDaemonErrors::Gather(HostAdapter& host) {
  memory_errors_.clear();

  ASSIGN_OR_RETURN(HostAdapter::CommandResult result,
                   host.RunCommand(absl::Seconds(kRunCommandTimeoutSecond),
                                   {"ras-mc-ctl", "--errors"}));

  if (result.exit_code != 0) {
    return absl::InternalError(
        absl::StrFormat("`ras-mc-ctl --errors` run failed, error_code: [%d]."
                        "\nstdout: [%s]\nstderr:[%s] ",
                        result.exit_code, result.stdout, result.stderr));
  }
  return ParseErrorReport(result.stdout);
}

absl::Status RasDaemonErrors::ParseErrorReport(absl::string_view ras_errors) {
  ParsingStep step = ParsingStep::MC_TITLE;

  for (absl::string_view line : absl::StrSplit(ras_errors, '\n')) {
    switch (step) {
      case ParsingStep::MC_TITLE: {
        ASSIGN_OR_RETURN(step, ParseMcTitle(line));
        break;
      }
      case ParsingStep::MC_RECORD: {
        ASSIGN_OR_RETURN(step, ParseMcRecord(line, memory_errors_));
        break;
      }
      case ParsingStep::FINISH: {
        return absl::OkStatus();
      }
    }
  }
  return absl::InternalError(
      "`ras-mc-ctl --errors`: parsed failed, unexpected end of file.");
}
}  // namespace ocpdiag::hwinterface::internal
