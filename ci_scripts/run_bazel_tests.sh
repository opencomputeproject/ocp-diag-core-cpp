#!/bin/bash

set -eu
bazel build ...
bazel test ... --test_output=errors
