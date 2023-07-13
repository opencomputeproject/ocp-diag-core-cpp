# Copyright 2022 Google LLC
#
# Use of this source code is governed by an MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT.

"""
Provides build rules for the OCPDiag hardware interface libraries.
"""

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "hw_info",
    srcs = ["hw_info.cc"],
    hdrs = ["hw_info.h"],
    deps = [
        "//ocpdiag/core/compat:status_macros",
        "//ocpdiag/core/hwinterface:identifier_cc_proto",
        "//ocpdiag/core/hwinterface/backends/lib:utils",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "hw_info_test",
    srcs = ["hw_info_test.cc"],
    data = [
    ],
    deps = [
        ":hw_info",
        "//ocpdiag/core/hwinterface:identifier_cc_proto",
        "//ocpdiag/core/testing:file_utils",
        "//ocpdiag/core/testing:proto_matchers",
        "//ocpdiag/core/testing:status_matchers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "utils",
    srcs = ["utils.cc"],
    hdrs = ["utils.h"],
    deps = [
        ":host_adapter",
        "//ocpdiag/core/compat:status_macros",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_protobuf//:protobuf",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "utils_test",
    size = "small",
    srcs = ["utils_test.cc"],
    deps = [
        ":fake_host_adapter",
        ":utils",
        "//ocpdiag/core/hwinterface:cpu_cc_proto",
        "//ocpdiag/core/hwinterface:service_cc_proto",
        "//ocpdiag/core/hwinterface/backends/lib:mock_host_adapter",
        "//ocpdiag/core/testing:parse_text_proto",
        "//ocpdiag/core/testing:status_matchers",
        "@com_google_absl//absl/status",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "cpu",
    hdrs = ["cpu.h"],
    deps = [
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "host_adapter",
    srcs = ["host_adapter.cc"],
    hdrs = ["host_adapter.h"],
    visibility = [
        "//ocpdiag:__subpackages__",
        "//platforms/testing/error_injection:__subpackages__",
    ],
    deps = [
        "//ocpdiag/core/compat:status_macros",
        "//ocpdiag/core/lib/off_dut_machine_interface:remote_cc",
        "//ocpdiag/core/lib/off_dut_machine_interface:remote_factory_cc",
        "@com_google_absl//absl/cleanup",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
        "@com_google_ecclesia//ecclesia/lib/apifs",
    ],
)

cc_test(
    name = "host_adapter_test",
    size = "small",
    srcs = ["host_adapter_test.cc"],
    deps = [
        ":host_adapter",
        "//ocpdiag/core/compat:status_macros",
        "//ocpdiag/core/lib/off_dut_machine_interface:mock_remote_cc",
        "//ocpdiag/core/lib/off_dut_machine_interface:remote_cc",
        "//ocpdiag/core/testing:status_matchers",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:cord",
        "@com_google_absl//absl/time",
        "@com_google_absl//absl/types:span",
        "@com_google_ecclesia//ecclesia/lib/file:test_filesystem",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_host_adapter",
    testonly = True,
    hdrs = ["mock_host_adapter.h"],
    visibility = ["//ocpdiag:__subpackages__"],
    deps = [
        ":host_adapter",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_ecclesia//ecclesia/lib/file:test_filesystem",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "fake_host_adapter",
    testonly = True,
    srcs = ["fake_host_adapter.cc"],
    hdrs = ["fake_host_adapter.h"],
    deps = [
        ":host_adapter",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "fake_host_adapter_test",
    size = "small",
    srcs = ["fake_host_adapter_test.cc"],
    deps = [
        ":fake_host_adapter",
        "//ocpdiag/core/testing:status_matchers",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/time",
        "@com_google_absl//absl/types:span",
        "@com_google_googletest//:gtest_main",
    ],
)
