// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/backends/lib/utils.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "ocpdiag/core/hwinterface/backends/lib/mock_host_adapter.h"
#include "ocpdiag/core/hwinterface/cpu.pb.h"
#include "ocpdiag/core/hwinterface/service.pb.h"
#include "ocpdiag/core/testing/parse_text_proto.h"
#include "ocpdiag/core/testing/status_matchers.h"

namespace ocpdiag::hwinterface {
namespace internal {
namespace {

using ::ocpdiag::testing::IsOkAndHolds;
using ::ocpdiag::testing::ParseTextProtoOrDie;
using ::ocpdiag::testing::StatusIs;
using ::testing::ContainerEq;
using ::testing::Eq;
using ::testing::HasSubstr;

TEST(InfoTypeHaveTest, EmpytInfoTypes) {
  GetCpuInfoRequest req;
  EXPECT_TRUE(InfoTypeHave(req.info_types(), cpu::InfoType::TOPOLOGY));
}

TEST(InfoTypeHaveTest, TypeInInfoTypes) {
  GetCpuInfoRequest req;
  req.add_info_types(cpu::InfoType::SIGNATURE);
  EXPECT_TRUE(InfoTypeHave(req.info_types(), cpu::InfoType::SIGNATURE));
}

TEST(InfoTypeHaveTest, TypeNotInInfoTypes) {
  GetCpuInfoRequest req;
  req.add_info_types(cpu::InfoType::SIGNATURE);
  EXPECT_FALSE(InfoTypeHave(req.info_types(), cpu::InfoType::TOPOLOGY));
}

TEST(InfoTypeHaveAnyTest, EmpytInfoTypes) {
  GetCpuInfoRequest req;
  EXPECT_TRUE(InfoTypeHave(req.info_types(), cpu::InfoType::TOPOLOGY));
}

TEST(InfoTypeHavAnyTest, TypesInInfoTypes) {
  GetCpuInfoRequest req;
  req.add_info_types(cpu::InfoType::SIGNATURE);
  EXPECT_TRUE(InfoTypeHaveAny(req.info_types(),
                              std::vector<int>{cpu::InfoType::SIGNATURE}));
}

TEST(InfoTypeHaveAnyTest, TypesNotInInfoTypes) {
  GetCpuInfoRequest req;
  req.add_info_types(cpu::InfoType::SIGNATURE);
  EXPECT_FALSE(InfoTypeHaveAny(req.info_types(),
                               std::vector<int>{cpu::InfoType::FREQUENCY}));
}

TEST(ParseRAngeListTest, ValidArgument) {
  EXPECT_THAT(ParseRangeList("0-1"),
              IsOkAndHolds(ContainerEq(std::vector<int>{0, 1})));
  EXPECT_THAT(ParseRangeList("2-2"),
              IsOkAndHolds(ContainerEq(std::vector<int>{2})));
  EXPECT_THAT(ParseRangeList("0-1,4,7-8,10"),
              IsOkAndHolds(ContainerEq(std::vector<int>{0, 1, 4, 7, 8, 10})));
}

TEST(ParseRAngeListTest, InvalidArgument) {
  EXPECT_THAT(ParseRangeList("1-2-3"),
              StatusIs(absl::StatusCode::kInvalidArgument));
  EXPECT_THAT(ParseRangeList("2-1"),
              StatusIs(absl::StatusCode::kInvalidArgument));
  EXPECT_THAT(ParseRangeList("1-4,3-6"),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(GrpcHostPortStringTest, Ipv4) {
  EXPECT_THAT(GrpcHostPortString("1.2.33.44", 443), Eq("1.2.33.44:443"));
}

TEST(GrpcHostPortStringTest, Ipv6) {
  EXPECT_THAT(GrpcHostPortString("::1", 443), Eq("[::1]:443"));
  EXPECT_THAT(GrpcHostPortString("[::1]", 443), Eq("[::1]:443"));
}

TEST(GrpcHostPortStringTest, Fqdn) {
  EXPECT_THAT(GrpcHostPortString("myhost", 443), Eq("dns:///myhost:443"));
  EXPECT_THAT(GrpcHostPortString("myhost.google.com", 443),
              Eq("dns:///myhost.google.com:443"));
}

}  // namespace
}  // namespace internal
}  // namespace ocpdiag::hwinterface
