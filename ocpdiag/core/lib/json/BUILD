# Copyright 2022 Google LLC
#
# Use of this source code is governed by an MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT.

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "json_proto_helpers",
    srcs = ["json_proto_helpers.cc"],
    hdrs = ["json_proto_helpers.h"],
    deps = [
        "//ocpdiag/core/compat:status_converters",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_test(
    name = "json_proto_helpers_test",
    srcs = ["json_proto_helpers_test.cc"],
    data = [
        "//ocpdiag/core/lib/json/testdata:bad_test_params.json",
        "//ocpdiag/core/lib/json/testdata:test_params.json",
    ],
    deps = [
        ":json_proto_helpers",
        "//ocpdiag/core/lib/json/testdata:test_params_cc_proto",
        "//ocpdiag/core/testing:file_utils",
        "//ocpdiag/core/testing:proto_matchers",
        "//ocpdiag/core/testing:status_matchers",
        "@com_google_absl//absl/status",
        "@com_google_googletest//:gtest_main",
    ],
)
