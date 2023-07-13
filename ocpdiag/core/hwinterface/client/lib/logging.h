// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_HWINTERFACE_CLIENT_LIB_LOGGING_H_
#define OCPDIAG_CORE_HWINTERFACE_CLIENT_LIB_LOGGING_H_

#include <functional>
#include <string>

#include "absl/log/log.h"
#include "absl/strings/string_view.h"

namespace ocpdiag::hwinterface::internal {

template <typename RequestType, typename ResponseType>
ResponseType HalLoggingHelper(std::function<ResponseType(RequestType)> callee,
                              absl::string_view callee_name, RequestType req,
                              int line = __builtin_LINE(),
                              const char* file_name = __builtin_FILE()) {
  LOG(INFO).AtLocation(file_name, line) << "[HAL] Calls: " << callee_name;
  LOG(INFO).AtLocation(file_name, line)
      << "[HAL] " << callee_name << " request: " << req.DebugString();
  ResponseType resp = callee(req);
  LOG(INFO).AtLocation(file_name, line)
      << "[HAL] " << callee_name << " response: "
      << (resp.ok() ? resp->DebugString() : resp.status().message());
  return resp;
}

}  // namespace ocpdiag::hwinterface::internal

#define OCPDIAG_CALL_BACKEND_API(backend, call, req) \
  ocpdiag::hwinterface::internal::HalLoggingHelper<  \
      decltype(req), decltype(backend->call(req))>( \
      [&](decltype(req) req_for_lambda) {           \
        return backend->call(req_for_lambda);       \
      },                                            \
      #call, req)

#endif  // OCPDIAG_CORE_HWINTERFACE_CLIENT_LIB_LOGGING_H_
