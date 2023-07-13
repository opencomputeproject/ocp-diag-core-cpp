// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/service_interface.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "ocpdiag/core/testing/status_matchers.h"

namespace ocpdiag {
namespace hwinterface {
namespace {

class FakeClient : public OCPDiagServiceInterface {
 public:
  FakeClient() = default;

  std::string Name() final { return "FakeClient"; }

  std::string GetBackendAddress() final { return "FakeClient Backend Address"; }

  absl::StatusOr<GetSensorsResponse> GetSensors(
      const GetSensorsRequest& req) override {
    return GetSensorsResponse();
  }
};

TEST(OCPDiagServiceInterface, ClientImplement) {
  EXPECT_OK(FakeClient().GetSensors({}));
}

TEST(OCPDiagServiceInterface, UnimplementedError) {
  auto resp = FakeClient().GetCpuInfo({});
  EXPECT_THAT(resp,
              ::ocpdiag::testing::StatusIs(absl::StatusCode::kUnimplemented));
  EXPECT_THAT(
      resp.status().message(),
      ::testing::ContainsRegex("GetCpuInfo is unimplemented by FakeClient"));
}

}  // namespace
}  // namespace hwinterface
}  // namespace ocpdiag
