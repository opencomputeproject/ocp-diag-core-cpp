// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <memory>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "ocpdiag/core/hwinterface/backends/host/host_backend.h"
#include "ocpdiag/core/hwinterface/service.pb.h"
#include "ocpdiag/core/hwinterface/service_interface.h"
#include "ocpdiag/core/testing/proto_matchers.h"
#include "ocpdiag/core/testing/status_matchers.h"

using ::ocpdiag::hwinterface::GetHwInfoRequest;
using ::ocpdiag::hwinterface::GetHwInfoResponse;
using ::ocpdiag::hwinterface::internal::HostBackend;
using ::ocpdiag::testing::EqualsProto;
using ::ocpdiag::testing::IsOkAndHolds;
using ::ocpdiag::testing::StatusIs;
using ::third_party::ocpdiag::results_pb::HardwareInfo;

namespace {

class GetHwInfoTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_OK_AND_ASSIGN(backend_, HostBackend::Create({}));
  }
  std::unique_ptr<ocpdiag::hwinterface::OCPDiagServiceInterface> backend_;
};

TEST_F(GetHwInfoTest, FailsToUnmarshall) {
  GetHwInfoRequest req;
  req.add_ids("bad_id_string");

  EXPECT_THAT(backend_->GetHwInfo(req),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST_F(GetHwInfoTest, EmptyCase) {
  EXPECT_THAT(backend_->GetHwInfo({}),
              IsOkAndHolds(EqualsProto(GetHwInfoResponse{})));
}

TEST_F(GetHwInfoTest, Succeeds) {
  GetHwInfoRequest req;
  std::string id = "%odata%name%arena%type";
  req.add_ids(id);
  GetHwInfoResponse want_resp;
  HardwareInfo& info = (*want_resp.mutable_hw_info())[id];
  info.set_name("name");
  info.set_arena("arena");
  info.set_part_type("type");
  info.mutable_component_location()->set_odata_id("odata");
  EXPECT_THAT(backend_->GetHwInfo(req), IsOkAndHolds(EqualsProto(want_resp)));
}

}  // namespace
