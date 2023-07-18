// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/backends/host/host_backend.h"

#include <cstdio>
#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "google/protobuf/timestamp.pb.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "ocpdiag/core/hwinterface/backends/host/fake_host_adapter.h"
#include "ocpdiag/core/hwinterface/backends/lib/mock_host_adapter.h"
#include "ocpdiag/core/hwinterface/error.pb.h"
#include "ocpdiag/core/hwinterface/service.pb.h"
#include "ocpdiag/core/testing/file_utils.h"
#include "ocpdiag/core/testing/parse_text_proto.h"
#include "ocpdiag/core/testing/proto_matchers.h"
#include "ocpdiag/core/testing/status_matchers.h"

namespace ocpdiag::hwinterface::internal {
namespace {

using ::ocpdiag::hwinterface::RebootRequest::RebootType::
    RebootRequest_RebootType_REBOOTTYPE_HARD;
using ::ocpdiag::hwinterface::RebootRequest::RebootType::
    RebootRequest_RebootType_REBOOTTYPE_SOFT;
using ::ocpdiag::hwinterface::RebootRequest::RebootType::
    RebootRequest_RebootType_REBOOTTYPE_UNSPECIFIED;
using ::ocpdiag::testing::EqualsProto;
using ::ocpdiag::testing::IsOkAndHolds;
using ::ocpdiag::testing::ParseTextProtoOrDie;
using ::ocpdiag::testing::StatusIs;
using ::testing::_;
using ::testing::NotNull;
using ::testing::Return;

// Returns the contents in `filename` under
// "ocpdiag/core/hwinterface/backends/host/testdata"
std::string GetTestDataContents(absl::string_view filename) {
  std::filesystem::path testdata_path(
      "ocpdiag/core/hwinterface/backends/host/testdata");
  return testutils::GetDataDependencyFileContents(
      (testdata_path.append(filename)).string());
}

TEST(HostBackend, ApiUnimplemented) {
  EntityConfiguration config;
  absl::StatusOr<std::unique_ptr<OCPDiagServiceInterface>> backend =
      HostBackend::Create(config);
  ASSERT_THAT(backend, IsOkAndHolds(NotNull()));

  EXPECT_THAT((*backend)->GetSensors(GetSensorsRequest()),
              StatusIs(absl::StatusCode::kUnimplemented,
                       "GetSensors is unimplemented by HostBackend"));
}

TEST(HostBackend, GetCpuInfo) {
  HostBackend backend(EntityConfiguration(),
                      std::make_unique<RealisticHostAdapter>());

  GetCpuInfoResponse expected =
      ParseTextProtoOrDie(GetTestDataContents("get_cpu_info/expected.textpb"));

  EXPECT_THAT(backend.GetCpuInfo(GetCpuInfoRequest()),
              IsOkAndHolds(EqualsProto(expected)));
}

TEST(GetErrors, ReturnsSuccessfullyWithTimestampFilter) {
  auto mock_host = std::make_unique<MockHostAdapter>();

  HostAdapter::CommandResult result{
      .exit_code = 0,
      .stdout = GetTestDataContents("get_errors/error.report"),
      .stderr = "",
  };

  EXPECT_CALL(*mock_host, RunCommand
              )
      .Times(1)
      .WillOnce(Return(result));

  HostBackend backend(EntityConfiguration{}, std::move(mock_host));

  GetErrorsRequest req;
  req.mutable_filter()->mutable_start_timestamp()->set_seconds(1622733217);
  req.mutable_filter()->mutable_end_timestamp()->set_seconds(1622733220);

  GetErrorsResponse expected = ParseTextProtoOrDie(
      GetTestDataContents("get_errors/expected_timestamp_filter.textpb"));

  EXPECT_THAT(backend.GetErrors(req), IsOkAndHolds(EqualsProto(expected)));
}

TEST(GetErrors, ReturnsSuccessfully) {
  auto mock_host = std::make_unique<MockHostAdapter>();

  HostAdapter::CommandResult result{
      .exit_code = 0,
      .stdout = GetTestDataContents("get_errors/error.report"),
      .stderr = "",
  };

  EXPECT_CALL(*mock_host, RunCommand
              )
      .Times(1)
      .WillOnce(Return(result));

  HostBackend backend(EntityConfiguration{}, std::move(mock_host));

  GetErrorsResponse expected = ParseTextProtoOrDie(
      GetTestDataContents("get_errors/expected_full.textpb"));

  EXPECT_THAT(backend.GetErrors(GetErrorsRequest()),
              IsOkAndHolds(EqualsProto(expected)));
}

TEST(GetErrors, TimeFilterStartTimestampGeaterThanEndTimestampInvalid) {
  HostBackend backend(EntityConfiguration{},
                      std::make_unique<MockHostAdapter>());

  GetErrorsRequest req;
  req.mutable_filter()->mutable_start_timestamp()->set_seconds(100);
  req.mutable_filter()->mutable_end_timestamp()->set_seconds(10);

  EXPECT_THAT(backend.GetErrors(req),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(GetErrors, TimeFilterStartTimestampEqualtoEndTimestampInvalid) {
  HostBackend backend(EntityConfiguration{},
                      std::make_unique<MockHostAdapter>());

  GetErrorsRequest req;
  req.mutable_filter()->mutable_start_timestamp()->set_seconds(100);
  req.mutable_filter()->mutable_end_timestamp()->set_seconds(100);

  EXPECT_THAT(backend.GetErrors(req),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(Reboot, SoftRebootReturnsSuccessfully) {
  auto mock_host = std::make_unique<MockHostAdapter>();
  HostAdapter::CommandResult result{
      .exit_code = 0,
      .stdout = "",
      .stderr = "",
  };

  EXPECT_CALL(
      *mock_host, RunCommand
      )
      .Times(1)
      .WillOnce(Return(result));
  HostBackend backend(EntityConfiguration{}, std::move(mock_host));

  RebootRequest req;
  req.set_type(RebootRequest_RebootType_REBOOTTYPE_SOFT);
  RebootResponse expected;
  EXPECT_THAT(backend.Reboot(req), IsOkAndHolds(EqualsProto(expected)));
}

TEST(Reboot, HardRebootUnimplemented) {
  HostBackend backend(EntityConfiguration{},
                      std::make_unique<MockHostAdapter>());

  RebootRequest req;
  req.set_type(RebootRequest_RebootType_REBOOTTYPE_HARD);
  EXPECT_THAT(backend.Reboot(req), StatusIs(absl::StatusCode::kUnimplemented));
}

TEST(Reboot, UnspecifiedRebootType) {
  HostBackend backend(EntityConfiguration{},
                      std::make_unique<MockHostAdapter>());

  RebootRequest req;
  req.set_type(RebootRequest_RebootType_REBOOTTYPE_UNSPECIFIED);
  EXPECT_THAT(backend.Reboot(req), StatusIs(absl::StatusCode::kInternal));
}

}  // namespace
}  // namespace ocpdiag::hwinterface::internal
