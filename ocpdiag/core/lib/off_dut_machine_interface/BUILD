# Copyright 2022 Google LLC
#
# Use of this source code is governed by an MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT.

# Common ocpdiag components rules.

load("//ocpdiag:python.bzl", "pytype_strict_library", pytype_strict_test = "pytype_strict_contrib_test")

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "remote_cc",
    hdrs = ["remote.h"],
    deps = [
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "mock_remote_cc",
    testonly = True,
    hdrs = ["mock_remote.h"],
    deps = [
        ":remote_cc",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "remote_factory_cc",
    srcs = [
        "remote_factory.cc",
    ],
    hdrs = [
        "remote_factory.h",
    ],
    deps = [
        ":remote_cc",
        "//ocpdiag/core/lib/off_dut_machine_interface/ssh:remote_ssh_cc",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "remote_factory_cc_test",
    srcs = ["remote_factory_test.cc"],
    deps = [
        ":remote_cc",
        ":remote_factory_cc",
        "//ocpdiag/core/lib/off_dut_machine_interface/ssh:remote_ssh_cc",
        "//ocpdiag/core/testing:status_matchers",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/status",
        "@com_google_googletest//:gtest_main",
    ],
)

pytype_strict_library(
    name = "remote_py",
    srcs = ["remote.py"],
    srcs_version = "PY3",
    deps = ["@six_archive//:six"],
)

pytype_strict_library(
    name = "remote_factory_py",
    srcs = ["remote_factory.py"],
    srcs_version = "PY3",
    deps = [
        ":remote_py",
        "//ocpdiag/core/lib/off_dut_machine_interface/ssh:remote_ssh_py",
        "@com_google_absl_py//absl/flags",
    ],
)
