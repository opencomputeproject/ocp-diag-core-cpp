// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_HWINTERFACE_BACKENDS_HOST_HOST_TEST_UTILS_H_
#define OCPDIAG_CORE_HWINTERFACE_BACKENDS_HOST_HOST_TEST_UTILS_H_

#include <string>

#include "absl/strings/string_view.h"

namespace ocpdiag::hwinterface::host::testing {

std::string GetTestDataContentsOrDie(absl::string_view filename);

}  // namespace ocpdiag::hwinterface::host::testing

#endif  // OCPDIAG_CORE_HWINTERFACE_BACKENDS_HOST_HOST_TEST_UTILS_H_
