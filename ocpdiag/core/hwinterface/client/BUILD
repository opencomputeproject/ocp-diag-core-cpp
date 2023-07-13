# Copyright 2022 Google LLC
#
# Use of this source code is governed by an MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT.

"""
Provides build rules for the OCPDiag Hardware Service Client.
"""

licenses(["notice"])

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "node_client_cc",
    srcs = ["node_client.cc"],
    hdrs = ["node_client.h"],
    deps = [
        "//ocpdiag/core/compat:status_macros",
        "//ocpdiag/core/hwinterface:config_cc_proto",
        "//ocpdiag/core/hwinterface:service_cc_proto",
        "//ocpdiag/core/hwinterface:service_flags",
        "//ocpdiag/core/hwinterface:service_interface",
        "//ocpdiag/core/hwinterface/backends/host:host_backend",
        "//ocpdiag/core/hwinterface/backends/lib:host_adapter",
        "//ocpdiag/core/hwinterface/client/lib:logging",
        "//ocpdiag/core/lib/json:json_proto_helpers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "node_client_test",
    size = "small",
    srcs = ["node_client_test.cc"],
    data = glob([
        "testdata/**",
    ]),
    deps = [
        ":node_client_cc",
        "//ocpdiag/core/hwinterface:config_cc_proto",
        "//ocpdiag/core/hwinterface:service_cc_proto",
        "//ocpdiag/core/hwinterface:service_flags",
        "//ocpdiag/core/hwinterface:service_interface",
        "//ocpdiag/core/testing:file_utils",
        "//ocpdiag/core/testing:proto_matchers",
        "//ocpdiag/core/testing:status_matchers",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:reflection",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_library(
    name = "mock_node_client",
    testonly = True,
    srcs = ["mock_node_client.cc"],
    hdrs = ["mock_node_client.h"],
    deps = [
        ":node_client_cc",
        "//ocpdiag/core/hwinterface:service_cc_proto",
        "//ocpdiag/core/hwinterface/backends/lib:mock_host_adapter",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "fake_node_client",
    testonly = 1,
    srcs = [
        "fake_node_client.cc",
    ],
    hdrs = [
        "fake_node_client.h",
    ],
    deps = [
        ":node_client_cc",
        "//ocpdiag/core/hwinterface:config_cc_proto",
        "//ocpdiag/core/hwinterface:service_cc_proto",
        "//ocpdiag/core/hwinterface/backends/lib:fake_host_adapter",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_test(
    name = "fake_node_client_test",
    size = "small",
    srcs = [
        "fake_node_client_test.cc",
    ],
    deps = [
        ":fake_node_client",
        "//ocpdiag/core/hwinterface:config_cc_proto",
        "//ocpdiag/core/hwinterface:service_cc_proto",
        "//ocpdiag/core/hwinterface/backends/lib:host_adapter",
        "//ocpdiag/core/testing:parse_text_proto",
        "//ocpdiag/core/testing:status_matchers",
        "@com_google_absl//absl/status",
        "@com_google_googletest//:gtest_main",
    ],
)
