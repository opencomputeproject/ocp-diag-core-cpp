// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/lib/json/json_proto_helpers.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "ocpdiag/core/lib/json/testdata/test_params.pb.h"
#include "ocpdiag/core/testing/file_utils.h"
#include "ocpdiag/core/testing/proto_matchers.h"
#include "ocpdiag/core/testing/status_matchers.h"

namespace ocpdiag {
namespace {

using ::ocpdiag::testdata::Params;
using ::ocpdiag::testing::EqualsProto;
using ::ocpdiag::testing::StatusIs;

TEST(JsonFileToMessageTest, BadFile) {
  Params params;
  EXPECT_THAT(JsonFileToMessage("/not/a/valid/path", &params),
              StatusIs(absl::StatusCode::kNotFound));
}

TEST(JsonFileToMessageTest, BadMessage) {
  Params params;
  std::string test_path = ocpdiag::testutils::GetDataDependencyFilepath(
      "ocpdiag/core/lib/json/testdata/bad_test_params.json");
  EXPECT_THAT(JsonFileToMessage(test_path.c_str(), &params),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(JsonFileToMessageTest, GoodMessage) {
  Params params;
  std::string test_path = ocpdiag::testutils::GetDataDependencyFilepath(
      "ocpdiag/core/lib/json/testdata/test_params.json");
  ASSERT_OK(JsonFileToMessage(test_path.c_str(), &params));
  EXPECT_EQ(params.foo(), "foo_string");
}

TEST(CombineFromJsonStreamTest, Merge) {
  Params params;
  params.set_foo("foo");
  std::string json = R"({"bar": 1})";
  google::protobuf::io::ArrayInputStream msg_stream(json.data(), json.size());
  ASSERT_OK(CombineFromJsonStream(&msg_stream, &params, /*overwrite=*/false));
  EXPECT_THAT(params, EqualsProto("foo: 'foo' bar: 1"));
}

TEST(CombineFromJsonStreamTest, Overwrite) {
  Params params;
  params.set_foo("foo");
  std::string json = "{}";  // an empty json object should clear all fields
  google::protobuf::io::ArrayInputStream msg_stream(json.data(), json.size());
  ASSERT_OK(CombineFromJsonStream(&msg_stream, &params, /*overwrite=*/true));
  EXPECT_THAT(params, EqualsProto(""));
}

TEST(CombineFromJsonStreamTest, NoOverwriteWithEmptyStream) {
  Params params;
  params.set_foo("foo");
  Params expected_output = params;
  std::string json = "";  // empty stream input should not mutate the proto
  google::protobuf::io::ArrayInputStream msg_stream(json.data(), json.size());
  ASSERT_OK(CombineFromJsonStream(&msg_stream, &params, /*overwrite=*/true));
  EXPECT_THAT(params, EqualsProto(expected_output));
}

TEST(CombineFromJsonStreamTest, WhitespaceStreamError) {
  Params params;
  std::string json = " ";
  google::protobuf::io::ArrayInputStream msg_stream(json.data(), json.size());
  auto status = CombineFromJsonStream(&msg_stream, &params, /*overwrite=*/true);
  EXPECT_FALSE(status.ok());
}

}  // namespace
}  // namespace ocpdiag
