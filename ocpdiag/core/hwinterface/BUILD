# Copyright 2022 Google LLC
#
# Use of this source code is governed by an MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT.

"""
BUILD rules for the OCPDiag Hardware Abstraction layer.

These APIs are experimental and may change at any time.
"""

load("@com_github_grpc_grpc//bazel:python_rules.bzl", "py_proto_library")

licenses(["notice"])

package(
    default_visibility = ["//visibility:public"],
)

proto_library(
    name = "service_proto",
    srcs = ["service.proto"],
    deps = [
        ":accelerator_proto",
        ":cpu_proto",
        ":ec_proto",
        ":eeprom_proto",
        ":error_proto",
        ":fan_proto",
        ":firmware_proto",
        ":gpu_proto",
        ":identifier_proto",
        ":interface_proto",
        ":memory_proto",
        ":networkinterface_proto",
        ":node_proto",
        ":psu_proto",
        ":securitychip_proto",
        ":sensor_proto",
        ":storage_proto",
        ":system_proto",
        "//ocpdiag/core/results/data_model:legacy_results_proto",
    ],
)

cc_proto_library(
    name = "service_cc_proto",
    deps = [":service_proto"],
)

proto_library(
    name = "config_proto",
    srcs = ["config.proto"],
    deps = ["@com_google_protobuf//:duration_proto"],
)

proto_library(
    name = "cpu_proto",
    srcs = ["cpu.proto"],
    deps = [":identifier_proto"],
)

proto_library(
    name = "memory_proto",
    srcs = ["memory.proto"],
    deps = [":identifier_proto"],
)

proto_library(
    name = "sensor_proto",
    srcs = ["sensor.proto"],
    deps = [":identifier_proto"],
)

proto_library(
    name = "interface_proto",
    srcs = ["interface.proto"],
    deps = [":identifier_proto"],
)

proto_library(
    name = "eeprom_proto",
    srcs = ["eeprom.proto"],
    deps = [
        ":identifier_proto",
    ],
)

proto_library(
    name = "error_proto",
    srcs = ["error.proto"],
    deps = [
        ":identifier_proto",
        ":interface_proto",
        "@com_google_protobuf//:timestamp_proto",
    ],
)

proto_library(
    name = "fan_proto",
    srcs = ["fan.proto"],
    deps = [":identifier_proto"],
)

proto_library(
    name = "identifier_proto",
    srcs = ["identifier.proto"],
)

proto_library(
    name = "system_proto",
    srcs = ["system.proto"],
    deps = [
        ":identifier_proto",
        "//ocpdiag/core/results/data_model:legacy_results_proto",
    ],
)

proto_library(
    name = "node_proto",
    srcs = ["node.proto"],
    deps = [":identifier_proto"],
)

proto_library(
    name = "securitychip_proto",
    srcs = ["securitychip.proto"],
    deps = [":identifier_proto"],
)

proto_library(
    name = "ec_proto",
    srcs = ["ec.proto"],
    deps = [":identifier_proto"],
)

proto_library(
    name = "firmware_proto",
    srcs = ["firmware.proto"],
    deps = [":identifier_proto"],
)

proto_library(
    name = "networkinterface_proto",
    srcs = ["networkinterface.proto"],
    deps = [":identifier_proto"],
)

proto_library(
    name = "storage_proto",
    srcs = ["storage.proto"],
    deps = [":identifier_proto"],
)

proto_library(
    name = "psu_proto",
    srcs = ["psu.proto"],
    deps = [":identifier_proto"],
)

proto_library(
    name = "gpu_proto",
    srcs = ["gpu.proto"],
    deps = [":identifier_proto"],
)

proto_library(
    name = "accelerator_proto",
    srcs = ["accelerator.proto"],
    deps = [":identifier_proto"],
)

cc_proto_library(
    name = "identifier_cc_proto",
    deps = [":identifier_proto"],
)

cc_proto_library(
    name = "cpu_cc_proto",
    deps = [":cpu_proto"],
)

cc_proto_library(
    name = "memory_cc_proto",
    deps = [":memory_proto"],
)

cc_proto_library(
    name = "sensor_cc_proto",
    deps = [":sensor_proto"],
)

cc_proto_library(
    name = "interface_cc_proto",
    deps = [":interface_proto"],
)

cc_proto_library(
    name = "eeprom_cc_proto",
    deps = [":eeprom_proto"],
)

cc_proto_library(
    name = "error_cc_proto",
    deps = [":error_proto"],
)

cc_proto_library(
    name = "fan_cc_proto",
    deps = [":fan_proto"],
)

cc_proto_library(
    name = "system_cc_proto",
    deps = [":system_proto"],
)

cc_proto_library(
    name = "node_cc_proto",
    deps = [":node_proto"],
)

cc_proto_library(
    name = "securitychip_cc_proto",
    deps = [":securitychip_proto"],
)

cc_proto_library(
    name = "ec_cc_proto",
    deps = [":ec_proto"],
)

cc_proto_library(
    name = "firmware_cc_proto",
    deps = [":firmware_proto"],
)

cc_proto_library(
    name = "networkinterface_cc_proto",
    deps = [":networkinterface_proto"],
)

cc_proto_library(
    name = "storage_cc_proto",
    deps = [":storage_proto"],
)

cc_proto_library(
    name = "psu_cc_proto",
    deps = [":psu_proto"],
)

cc_proto_library(
    name = "gpu_cc_proto",
    deps = [":gpu_proto"],
)

cc_proto_library(
    name = "accelerator_cc_proto",
    deps = [":accelerator_proto"],
)

cc_proto_library(
    name = "config_cc_proto",
    deps = [":config_proto"],
)

cc_library(
    name = "service_interface",
    hdrs = ["service_interface.h"],
    deps = [
        ":config_cc_proto",
        ":service_cc_proto",
        "//ocpdiag/core/results/data_model:legacy_results_cc_proto",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "service_flags",
    srcs = ["service_flags.cc"],
    hdrs = ["service_flags.h"],
    deps = [
        ":config_cc_proto",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:marshalling",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
        "@com_google_protobuf//:protobuf",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "service_interface_test",
    size = "small",
    srcs = ["service_interface_test.cc"],
    deps = [
        ":service_interface",
        "//ocpdiag/core/testing:status_matchers",
        "@com_google_absl//absl/status",
        "@com_google_googletest//:gtest_main",
    ],
)

py_proto_library(
    name = "service_py_pb2",
    deps = [":service_proto"],
)

py_proto_library(
    name = "config_py_pb2",
    deps = [":config_proto"],
)

py_proto_library(
    name = "cpu_py_pb2",
    deps = [":cpu_proto"],
)

py_proto_library(
    name = "memory_py_proto",
    deps = [":memory_proto"],
)

py_proto_library(
    name = "sensor_py_proto",
    deps = [":sensor_proto"],
)

py_proto_library(
    name = "interface_py_proto",
    deps = [":interface_proto"],
)

py_proto_library(
    name = "eeprom_py_proto",
    deps = [":eeprom_proto"],
)

py_proto_library(
    name = "error_py_proto",
    deps = [":error_proto"],
)

py_proto_library(
    name = "fan_py_proto",
    deps = [":fan_proto"],
)

py_proto_library(
    name = "identifier_py_pb2",
    deps = [":identifier_proto"],
)

py_proto_library(
    name = "system_py_pb2",
    deps = [":system_proto"],
)

py_proto_library(
    name = "node_py_pb2",
    deps = [":node_proto"],
)

py_proto_library(
    name = "securitychip_py_pb2",
    deps = [":securitychip_proto"],
)

py_proto_library(
    name = "ec_py_pb2",
    deps = [":ec_proto"],
)

py_proto_library(
    name = "firmware_py_pb2",
    deps = [":firmware_proto"],
)

py_proto_library(
    name = "networkinterface_py_pb2",
    deps = [":networkinterface_proto"],
)

py_proto_library(
    name = "storage_py_pb2",
    deps = [":storage_proto"],
)

py_proto_library(
    name = "psu_py_pb2",
    deps = [":psu_proto"],
)

py_proto_library(
    name = "gpu_py_pb2",
    deps = [":gpu_proto"],
)

py_proto_library(
    name = "accelerator_py_pb2",
    deps = [":accelerator_proto"],
)
