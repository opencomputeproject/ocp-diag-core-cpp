# Copyright 2022 Google LLC
#
# Use of this source code is governed by an MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT.

"""
BUILD rules for the OCPDiag Hardware Interface Libraries.
"""

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "identifier_utils",
    srcs = ["identifier_utils.cc"],
    hdrs = ["identifier_utils.h"],
    deps = [
        "//ocpdiag/core/compat:status_macros",
        "//ocpdiag/core/hwinterface:identifier_cc_proto",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_protobuf//:protobuf",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "identifier_utils_test",
    srcs = ["identifier_utils_test.cc"],
    deps = [
        ":identifier_utils",
        "//ocpdiag/core/hwinterface:identifier_cc_proto",
        "//ocpdiag/core/testing:status_matchers",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_library(
    name = "proto_utils",
    hdrs = ["proto_utils.h"],
    deps = ["@com_google_protobuf//:protobuf"],
)
