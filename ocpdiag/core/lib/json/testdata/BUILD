# Copyright 2022 Google LLC
#
# Use of this source code is governed by an MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT.

load("//ocpdiag:packaging.bzl", "descriptor_set")
load("@com_github_grpc_grpc//bazel:python_rules.bzl", "py_proto_library")

package(default_visibility = ["//visibility:public"])

exports_files(
    srcs = [
        "bad_test_params.json",
        "test_params.json",
    ],
)

proto_library(
    name = "test_params",
    testonly = True,
    srcs = ["test_params.proto"],
)

cc_proto_library(
    name = "test_params_cc_proto",
    testonly = True,
    deps = [":test_params"],
)

py_proto_library(
    name = "test_params_py_pb2",
    testonly = True,
    deps = [":test_params"],
)

descriptor_set(
    name = "test_params_descriptor",
    testonly = True,
    deps = ["test_params"],
)
