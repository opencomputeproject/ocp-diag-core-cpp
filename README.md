# ocp-diag-core-cpp (ocpdiag)

The [**OCP Test & Validation Initiative**](https://github.com/opencomputeproject/ocp-diag-core) is a collaboration between datacenter hyperscalers having the goal of standardizing aspects of the hardware validation/diagnosis space, along with providing necessary tooling to enable both diagnostic developers and executors to leverage these interfaces.

The **ocp-diag-core-cpp** repo is an open and multi-node friendly hardware diagnostics framework. The framework provides a set of core libraries with a multi-language solution based on C++ for creating portable diagnostics that can integrate into a number of different test platforms.

Furter on, the short name **OCPDiag** is used in place of **ocp-diag-core-cpp**.

**OCPDiag** is *not* a [test executive](https://en.wikipedia.org/wiki/Test_execution_engine). The framework provides test executives with a single integration point for all OCPDiag compatible diagnostics. The goal is to eliminate the wrapping effort required in porting other diagnostics to different execution environments and result schemas.

For diagnostics that run on multi-node systems, OCPDiag offers configuration options for targeting different endpoints and backends.

## Overview

**OCPDiag** diagnostics can run on a device under test (DUT) or off the DUT. After an **OCPDiag** type diag is built and installed, a test executive or a human needs to start the test and collect its results.

**OCPDiag** focuses on three main topics: Parameter Model, Results Model, and Hardware Interface.

This documentation provides a reference C++ implementation of the [OCP T&V schema](https://github.com/opencomputeproject/ocp-diag-core/tree/main/json_spec), and example usage for each of these topics. Each set of the core libraries can be imported individually without affecting the usages of other subdomains.

The following paragraphs use a [full_spec example](./ocpdiag/core/examples/full_spec/) to introduce the build rules, the models, and hardware interface.

## Running OCPDiag

A OCPDiag executable is a package that wraps a bash script with runfiles, including:

*   OCPDiag parameter launcher
*   default parameters
*   parameter descriptor
*   test main binary and their dependencies

The GitHub repo contains an `ocpdiag_test_pkg` Bazel rule for building the OCPDiag target and a full_spec example you can use to explore the build rules.

### Prerequisites

This workflow uses Bazel to build and test OCPDiag diagnostics. You must have Bazel v4.0.0 or greater installed. See also Installing Bazel [instructions](https://docs.bazel.build/versions/main/install.html).

### Configuring the Bazel WORKSPACE

Add a `WORKSPACE` configuration file in the top-level repository where Bazel runs. This enables Bazel to load **OCPDiag** core libraries.

```WORKSPACE
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "ocpdiag",
    commit = <pick a commit hash, put here in quotes>,
    remote = "https://github.com/opencomputeproject/ocp-diag-core-cpp",
)

load("@ocpdiag//ocpdiag:build_deps.bzl", "load_deps")
load_deps()

load("@ocpdiag//ocpdiag:secondary_build_deps.bzl", "load_secondary_deps")
load_secondary_deps()

load("@ocpdiag//ocpdiag:tertiary_build_deps.bzl", "load_tertiary_deps")
load_tertiary_deps()
```

### Create a test BUILD file

In the `BUILD` file, define a binary, a parameter proto, and a default parameter JSON file as the receipes to build the **OCPDiag** executable.

The following is an skeleton of the build receipts. For context, see the full [BUILD file](./ocpdiag/core/examples/full_spec/BUILD):

```BUILD
load("@ocpdiag//ocpdiag/core:ocpdiag.bzl", "ocpdiag_test_pkg")

# Proto parameter library.
proto_library(
    name = "params_proto",
    srcs = ["params.proto"],
)

cc_proto_library(
    name = "params_cc_proto",
    deps = [":params_proto"],
)

cc_binary(
    name = "full_spec",
    srcs = ["full_spec_main.cc"],
    deps = [
        "@com_google_absl//absl/flags:parse",
        ":params_cc_proto",
    ],
)

# OCPDiag executable.
ocpdiag_test_pkg(
    name = "ocpdiag_full_spec",
    binary = ":full_spec",
    json_defaults = "params.json",
    params_proto = ":params_proto",
)

```
### C++ Version Requirements

The OCPDiag project must be built with C++17. This can be accomplished by adding `--cxxopt='-std=c++17'` to any bazel build command. It may be more convenient to add a [`.bazelrc` file](https://docs.bazel.build/versions/main/guide.html#bazelrc-the-bazel-configuration-file) to the top level directory of your project, with the following line:

```
build --cxxopt='-std=c++17'
```

which will apply the option to all build commands.

### Build the OCPDiag target.

The following command line builds the **OCPDiag** executable.

```shell
<your_terminal>$ > bazel build //ocpdiag/core/examples/full_spec:full_spec --cxxopt='-std=c++17'
INFO: Analyzed target //ocpdiag/core/examples/full_spec:full_spec (127 packages loaded, 4413 targets configured).
INFO: Found 1 target...
Target //ocpdiag/core/examples/full_spec:full_spec up-to-date:
  bazel-bin/ocpdiag/core/examples/full_spec/full_spec
INFO: Elapsed time: 0.198s, Critical Path: 0.00s
INFO: 1 process: 1 internal.
INFO: Build completed successfully, 1 total action

# running the built binary
$ ./bazel-bin/ocpdiag/core/examples/full_spec/full_spec
{"schemaVersion":{"major":2,"minor":0},"sequenceNumber":0,"timestamp":"2023-06-10T15:32:24Z"}
{"testRunArtifact":{"log":{"severity":"INFO","message":"Adding log before test start."}},"sequenceNumber":1,"timestamp":"2023-06-10T15:32:24Z"}
{"testRunArtifact":{"log":{"severity":"WARNING","message":"This is a warning log."}},"sequenceNumber":2,"timestamp":"2023-06-10T15:32:24Z"}
{"testRunArtifact":{"error":{"symptom":"pre-start-error","message":"This would be an error that occurs before starting the test, usually when gathering DUT info.","softwareInfoIds":[]}},"sequenceNumber":3,"timestamp":"2023-06-10T15:32:24Z"}
[...]
```

The executable appears under `./bazel-bin/ocpdiag/core/examples/full_spec/full_spec` as indicated by bazel logs.

## Contact information

**Team:** ocp-test-validation@OCP-All.groups.io

**Code reviews:** ocpdiag-core-team+reviews@google.com
