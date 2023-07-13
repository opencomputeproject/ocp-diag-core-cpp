# Copyright 2022 Google LLC
#
# Use of this source code is governed by an MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT.

"""
Provides build rules for the OCPDiag hardware host backend.
"""

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "host_backend",
    srcs = [
        "host_backend.cc",
        "memory.cc",
        "node_info.cc",
        "storage_info.cc",
    ],
    hdrs = ["host_backend.h"],
    deps = [
        ":cpu",
        ":error",
        "//ocpdiag/core/compat:status_macros",
        "//ocpdiag/core/hwinterface:config_cc_proto",
        "//ocpdiag/core/hwinterface:cpu_cc_proto",
        "//ocpdiag/core/hwinterface:error_cc_proto",
        "//ocpdiag/core/hwinterface:identifier_cc_proto",
        "//ocpdiag/core/hwinterface:memory_cc_proto",
        "//ocpdiag/core/hwinterface:node_cc_proto",
        "//ocpdiag/core/hwinterface:service_cc_proto",
        "//ocpdiag/core/hwinterface:service_flags",
        "//ocpdiag/core/hwinterface:service_interface",
        "//ocpdiag/core/hwinterface:storage_cc_proto",
        "//ocpdiag/core/hwinterface/backends/lib:host_adapter",
        "//ocpdiag/core/hwinterface/backends/lib:hw_info",
        "//ocpdiag/core/hwinterface/backends/lib:utils",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
        "@com_google_ecclesia//ecclesia/lib/smbios:reader",
        "@com_google_ecclesia//ecclesia/lib/smbios:structures_emb",
        "@json",
    ],
)

cc_test(
    name = "hwinfo_test",
    srcs = ["hwinfo_test.cc"],
    deps = [
        ":host_backend",
        "//ocpdiag/core/hwinterface:service_cc_proto",
        "//ocpdiag/core/hwinterface:service_interface",
        "//ocpdiag/core/testing:proto_matchers",
        "//ocpdiag/core/testing:status_matchers",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "host_backend_test",
    size = "small",
    srcs = [
        "host_backend_test.cc",
        "memory_test.cc",
    ],
    data = glob(["testdata/*/*"]),
    deps = [
        ":fake_host_adapter",
        ":host_backend",
        "//ocpdiag/core/hwinterface:error_cc_proto",
        "//ocpdiag/core/hwinterface:memory_cc_proto",
        "//ocpdiag/core/hwinterface:service_cc_proto",
        "//ocpdiag/core/hwinterface/backends/lib:mock_host_adapter",
        "//ocpdiag/core/testing:file_utils",
        "//ocpdiag/core/testing:parse_text_proto",
        "//ocpdiag/core/testing:proto_matchers",
        "//ocpdiag/core/testing:status_matchers",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@com_google_absl//absl/types:span",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "fake_host_adapter",
    testonly = True,
    srcs = ["fake_host_adapter.cc"],
    hdrs = ["fake_host_adapter.h"],
    data = glob(["testdata/fake_host_adapter/*"]),
    deps = [
        "//ocpdiag/core/hwinterface/backends/lib:fake_host_adapter",
        "//ocpdiag/core/testing:file_utils",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "cpu",
    srcs = ["cpu.cc"],
    hdrs = ["cpu.h"],
    deps = [
        "//ocpdiag/core/compat:status_macros",
        "//ocpdiag/core/hwinterface:cpu_cc_proto",
        "//ocpdiag/core/hwinterface/backends/lib:cpu",
        "//ocpdiag/core/hwinterface/backends/lib:host_adapter",
        "//ocpdiag/core/hwinterface/backends/lib:utils",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "cpu_test",
    size = "small",
    srcs = ["cpu_test.cc"],
    data = glob(["testdata/cpu_signature_test/*"]),
    deps = [
        ":cpu",
        ":fake_host_adapter",
        "//ocpdiag/core/hwinterface:cpu_cc_proto",
        "//ocpdiag/core/testing:file_utils",
        "//ocpdiag/core/testing:status_matchers",
        "@com_google_absl//absl/status",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "error",
    srcs = ["error.cc"],
    hdrs = ["error.h"],
    deps = [
        "//ocpdiag/core/compat:status_macros",
        "//ocpdiag/core/hwinterface/backends/lib:host_adapter",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "error_test",
    size = "small",
    srcs = ["error_test.cc"],
    data = glob(["testdata/get_errors/*"]),
    deps = [
        ":error",
        "//ocpdiag/core/testing:file_utils",
        "//ocpdiag/core/testing:status_matchers",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "storage_info_test",
    size = "small",
    srcs = ["storage_info_test.cc"],
    data = glob(["testdata/get_storage_info/*"]),
    deps = [
        ":host_backend",
        ":host_test_utils",
        "//ocpdiag/core/hwinterface:service_cc_proto",
        "//ocpdiag/core/hwinterface:storage_cc_proto",
        "//ocpdiag/core/hwinterface/backends/lib:mock_host_adapter",
        "//ocpdiag/core/testing:parse_text_proto",
        "//ocpdiag/core/testing:proto_matchers",
        "//ocpdiag/core/testing:status_matchers",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "node_info_test",
    size = "small",
    srcs = ["node_info_test.cc"],
    deps = [
        ":host_backend",
        "//ocpdiag/core/hwinterface:service_cc_proto",
        "//ocpdiag/core/hwinterface/backends/lib:mock_host_adapter",
        "//ocpdiag/core/testing:proto_matchers",
        "//ocpdiag/core/testing:status_matchers",
        "@com_google_absl//absl/status",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "host_test_utils",
    testonly = True,
    srcs = ["host_test_utils.cc"],
    hdrs = ["host_test_utils.h"],
    deps = [
        "//ocpdiag/core/testing:file_utils",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_ecclesia//ecclesia/lib/apifs",
    ],
)
