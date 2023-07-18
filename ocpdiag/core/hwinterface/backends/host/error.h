// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_HWINTERFACE_BACKENDS_HOST_ERROR_H_
#define OCPDIAG_CORE_HWINTERFACE_BACKENDS_HOST_ERROR_H_

#include <ostream>
#include <string>
#include <vector>

#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "ocpdiag/core/hwinterface/backends/lib/host_adapter.h"

namespace ocpdiag::hwinterface::internal {

struct Error {
  absl::Time timestamp;
  int count;
  bool is_correctable;
  bool is_fatal;
  std::string label;
  std::string debuginfo;
};

inline bool operator==(const Error& lhs, const Error& rhs) {
  return (lhs.timestamp == rhs.timestamp) && (lhs.count == rhs.count) &&
         (lhs.is_correctable == rhs.is_correctable) &&
         (lhs.is_fatal == rhs.is_fatal) && (lhs.label == rhs.label) &&
         (lhs.debuginfo == rhs.debuginfo);
}

inline bool operator!=(const Error& lhs, const Error& rhs) {
  return !(lhs == rhs);
}

inline std::ostream& operator<<(std::ostream& os, const Error& err) {
  return os << "timestamp: " << err.timestamp << std::endl
            << "count: " << err.count << std::endl
            << "is_correctable: " << err.is_correctable << std::endl
            << "is_fatal: " << err.is_fatal << std::endl
            << "label: " << err.label << std::endl
            << "debuginfo: " << err.debuginfo;
}

// RasDaemonErrors gathers errors from rasdaemon.
class RasDaemonErrors {
 public:
  // Gathers errors from rasdaemon.
  // `ras-mc-ctl --errors` and parse output into structured.
  // `ras-mc-ctl --errors` will be executed every call.
  absl::Status Gather(HostAdapter& host);

  // Parses the `ras-mc-ctl --errors` content, `ras_errors`, into structured.
  absl::Status ParseErrorReport(absl::string_view ras_errors);

  // Memory controller errors.
  const std::vector<Error>& memory_errors() { return memory_errors_; }

 private:
  std::vector<Error> memory_errors_;
};

}  // namespace ocpdiag::hwinterface::internal
#endif  // OCPDIAG_CORE_HWINTERFACE_BACKENDS_HOST_ERROR_H_
