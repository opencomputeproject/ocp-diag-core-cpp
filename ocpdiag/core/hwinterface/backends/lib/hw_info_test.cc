// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/backends/lib/hw_info.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "ocpdiag/core/hwinterface/identifier.pb.h"
#include "ocpdiag/core/testing/file_utils.h"
#include "ocpdiag/core/testing/proto_matchers.h"
#include "ocpdiag/core/testing/status_matchers.h"

namespace ocpdiag::hwinterface {
namespace {

using ::ocpdiag::testing::EqualsProto;
using ::ocpdiag::testing::IsOkAndHolds;
using ::ocpdiag::testing::StatusIs;
using ::ocpdiag::testutils::GetDataDependencyFilepath;
using ::testing::HasSubstr;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

TEST(HwInterfaceHwInfoLibTest, PopulateIdAllFields) {
  Identifier identifier;
  identifier.set_name("name");
  identifier.set_devpath("devpath");
  identifier.set_odata_id("odata_id");
  identifier.set_arena("arena");
  identifier.set_type("type");
  PopulateId(identifier);
  ASSERT_EQ(identifier.id(), "devpath%odata_id%name%arena%type");
}

TEST(HwInterfaceHwInfoLibTest, PopulateIdPartialFields) {
  Identifier identifier;
  identifier.set_name("name");
  identifier.set_devpath("devpath");
  PopulateId(identifier);
  ASSERT_EQ(identifier.id(), "devpath%%name%%");
}

TEST(HwInterfaceHwInfoLibTest, PopulateIdNoFields) {
  Identifier identifier;
  PopulateId(identifier);
  ASSERT_EQ(identifier.id(), "%%%%");
}

TEST(UnmarshalIdentifierTest, AllFieldsId) {
  Identifier identifier;
  identifier.set_name("name");
  identifier.set_devpath("devpath");
  identifier.set_odata_id("odata_id");
  identifier.set_arena("arena");
  identifier.set_type("type");
  PopulateId(identifier);

  EXPECT_THAT(UnmarshalIdentifier("devpath%odata_id%name%arena%type"),
              IsOkAndHolds(EqualsProto(identifier)));
}

TEST(UnmarshalIdentifierTest, PartialFields) {
  Identifier identifier;
  identifier.set_name("name");
  identifier.set_devpath("devpath");
  PopulateId(identifier);

  EXPECT_THAT(UnmarshalIdentifier("devpath%%name%%"),
              IsOkAndHolds(EqualsProto(identifier)));
}

TEST(UnmarshalIdentifierTest, InvalidFormat) {
  EXPECT_THAT(
      UnmarshalIdentifier("devpath%%name"),
      StatusIs(absl::StatusCode::kInvalidArgument, HasSubstr("devpath%%name")));
}

}  // namespace
}  // namespace ocpdiag::hwinterface
