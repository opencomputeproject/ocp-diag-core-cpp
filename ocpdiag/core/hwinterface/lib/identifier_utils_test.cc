// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/lib/identifier_utils.h"

#include "google/protobuf/text_format.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "ocpdiag/core/hwinterface/identifier.pb.h"
#include "ocpdiag/core/testing/status_matchers.h"

namespace ocpdiag::hwinterface {

namespace {

using ::ocpdiag::testing::StatusIs;

TEST(IdentifierMatch, InvalidMatcher) {
  Filter filter;
  filter.mutable_id_regex()->set_name("*");
  EXPECT_THAT(IdentifierMatcher::Create(filter),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(IdentifierMatch, EmptyMatchesEverything) {
  Identifier id;
  absl::StatusOr<IdentifierMatcher> matcher =
      IdentifierMatcher::Create(Filter());
  ASSERT_OK(matcher);

  // Empty filter matches empty identifier.
  EXPECT_TRUE(matcher->Match(id));

  // Empty filter matches non-empty identifier.
  id.set_name("name");
  EXPECT_TRUE(matcher->Match(id));
}

TEST(IdentifierMatch, OperatorOr) {
  Identifier id;
  id.set_name("name");
  id.set_devpath("/path/to/devpath");
  Filter filter;
  filter.set_op(Filter::OR);

  // Adds one unmatched filter.
  filter.mutable_id_regex()->set_name("not_match");
  absl::StatusOr<IdentifierMatcher> matcher = IdentifierMatcher::Create(filter);
  ASSERT_OK(matcher);
  EXPECT_FALSE(matcher->Match(id));

  // Adds another matched filter.
  filter.mutable_id_regex()->set_devpath("devpath");
  matcher = IdentifierMatcher::Create(filter);
  EXPECT_TRUE(matcher->Match(id));
}

TEST(IdentifierMatch, OperatorAnd) {
  Identifier id;
  id.set_name("name");
  id.set_devpath("/path/to/devpath");
  Filter filter;
  filter.set_op(Filter::AND);

  // Adds one matched filter.
  filter.mutable_id_regex()->set_name("name");
  absl::StatusOr<IdentifierMatcher> matcher = IdentifierMatcher::Create(filter);
  ASSERT_OK(matcher);
  EXPECT_TRUE(matcher->Match(id));

  // Adds another unmatched filter.
  filter.mutable_id_regex()->set_devpath("not_match");
  matcher = IdentifierMatcher::Create(filter);
  ASSERT_OK(matcher);
  EXPECT_FALSE(matcher->Match(id));
}

TEST(IdentifierMatchersUnion, InvalidMatcher) {
  google::protobuf::RepeatedPtrField<Filter> filters;

  filters.Add()->mutable_id_regex()->set_name("*");

  EXPECT_THAT(IdentifierMatchersUnion::Create(filters),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(IdentifierMatchsUnion, EmptyMatchesEverything) {
  google::protobuf::RepeatedPtrField<Filter> filters;
  Identifier id;

  absl::StatusOr<IdentifierMatchersUnion> matchers =
      IdentifierMatchersUnion::Create(filters);
  ASSERT_OK(matchers);

  // Empty filter matches empty identifier.
  EXPECT_TRUE(matchers->Match(id));

  // Empty filter matches non-empty identifier.
  id.set_name("name");
  EXPECT_TRUE(matchers->Match(id));
}

TEST(IdentifierMatchsUnion, MatchIdentifier) {
  google::protobuf::RepeatedPtrField<Filter> filters;
  filters.Add()->mutable_id_regex()->set_name("name1");
  filters.Add()->mutable_id_regex()->set_name("name2");

  absl::StatusOr<IdentifierMatchersUnion> matchers =
      IdentifierMatchersUnion::Create(filters);
  ASSERT_OK(matchers);

  Identifier id;
  id.set_name("name2");

  EXPECT_TRUE(matchers->Match(id));
}

TEST(IdentifierMatchsUnion, NotMatchIdentifier) {
  google::protobuf::RepeatedPtrField<Filter> filters;
  filters.Add()->mutable_id_regex()->set_name("name1");
  filters.Add()->mutable_id_regex()->set_name("name2");

  absl::StatusOr<IdentifierMatchersUnion> matchers =
      IdentifierMatchersUnion::Create(filters);
  ASSERT_OK(matchers);

  Identifier id;
  id.set_name("name3");

  EXPECT_FALSE(matchers->Match(id));
}

}  // namespace

}  // namespace ocpdiag::hwinterface
