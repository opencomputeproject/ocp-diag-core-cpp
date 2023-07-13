// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_HWINTERFACE_BACKENDS_LIB_CPU_H_
#define OCPDIAG_CORE_HWINTERFACE_BACKENDS_LIB_CPU_H_

#include "absl/strings/string_view.h"

namespace ocpdiag::hwinterface {
namespace internal {

inline constexpr absl::string_view kVendorUnknown = "Unknown";
inline constexpr absl::string_view kVendorIntel = "Intel";
inline constexpr absl::string_view kVendorAmd = "AMD";
inline constexpr absl::string_view kVendorCyrix = "CYRIX";
inline constexpr absl::string_view kVendorCentaur = "Centaur";
inline constexpr absl::string_view kVendorNextGen = "NextGen";
inline constexpr absl::string_view kVendorTmx86 = "TMx86";
inline constexpr absl::string_view kVendorRise = "RISE";
inline constexpr absl::string_view kVendorUmc = "UMC";
inline constexpr absl::string_view kVendorIbm = "IBM";
inline constexpr absl::string_view kVendorApm = "APM";
inline constexpr absl::string_view kVendorQualcomm = "Qualcomm";
inline constexpr absl::string_view kVendorArm = "ARM";
inline constexpr absl::string_view kVendorAmpere = "Ampere";

}  // namespace internal
}  // namespace ocpdiag::hwinterface

#endif  // OCPDIAG_CORE_HWINTERFACE_BACKENDS_LIB_CPU_H_
