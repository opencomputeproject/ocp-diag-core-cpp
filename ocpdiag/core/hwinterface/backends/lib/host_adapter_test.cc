// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/backends/lib/host_adapter.h"

#include <filesystem>
#include <memory>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/strings/cord.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "absl/types/span.h"
#include "ecclesia/lib/file/test_filesystem.h"
#include "ocpdiag/core/lib/off_dut_machine_interface/mock_remote.h"
#include "ocpdiag/core/lib/off_dut_machine_interface/remote.h"
#include "ocpdiag/core/testing/status_matchers.h"

namespace ocpdiag::hwinterface {

namespace {

using ::ocpdiag::testing::IsOkAndHolds;
using ::ocpdiag::testing::StatusIs;
using ::testing::_;
using ::testing::Eq;
using ::testing::HasSubstr;
using ::testing::Return;

TEST(LocalHostAdapter, RunCommandSuccessed) {
  LocalHostAdapter local;

  absl::StatusOr<HostAdapter::CommandResult> result =
      local.RunCommand(absl::Minutes(1), {"echo", "-n", "hi"});
  ASSERT_OK(result);
  EXPECT_EQ(result.value().exit_code, 0);
  EXPECT_EQ(result.value().stdout, "hi");
  EXPECT_EQ(result.value().stderr, "");
}

TEST(LocalHostAdapter, RunCommandFailed) {
  LocalHostAdapter local;

  absl::StatusOr<HostAdapter::CommandResult> result =
      local.RunCommand(absl::Minutes(1), {"ls", "no-such-file"});
  ASSERT_OK(result);
  EXPECT_EQ(result.value().exit_code, 2);
  EXPECT_EQ(result.value().stdout, "");
  EXPECT_THAT(result.value().stderr, HasSubstr("cannot access"));
}

TEST(LocalHostAdapter, RunCommandNoSuchCommand) {
  LocalHostAdapter local;

  absl::StatusOr<HostAdapter::CommandResult> result =
      local.RunCommand(absl::Minutes(1), {"no-such-command"});
  ASSERT_OK(result);
  EXPECT_EQ(result.value().exit_code, 127);
  EXPECT_EQ(result.value().stdout, "");
  EXPECT_EQ(result.value().stderr, "");
}

TEST(LocalHostAdapter, RunCommandNotExecutable) {
  LocalHostAdapter local;

  absl::StatusOr<HostAdapter::CommandResult> result =
      local.RunCommand(absl::Minutes(1), {"/tmp"});
  ASSERT_OK(result);
  EXPECT_EQ(result.value().exit_code, 126);
  EXPECT_EQ(result.value().stdout, "");
  EXPECT_EQ(result.value().stderr, "");
}

TEST(LocalHostAdapter, RunCommandTimeout) {
  LocalHostAdapter local;

  absl::StatusOr<HostAdapter::CommandResult> result =
      local.RunCommand(absl::Seconds(1), {"sleep", "3"});
  EXPECT_EQ(result.status().code(), absl::StatusCode::kDeadlineExceeded)
      << result.status();
}

TEST(LocalHostAdapter, RunCommandEmpty) {
  LocalHostAdapter local;

  absl::StatusOr<HostAdapter::CommandResult> result =
      local.RunCommand(absl::Seconds(1), {});
  EXPECT_EQ(result.status().code(), absl::StatusCode::kInvalidArgument)
      << result.status();
}

TEST(LocalHostAdapter, FileReadWrite) {
  LocalHostAdapter local;
  std::string test_file =
      ecclesia::GetTestTempdirPath("file_read_write_test.file");

  constexpr absl::string_view kTestData = "Hello OCPDiag.";
  EXPECT_OK(local.Write(test_file, kTestData));
  EXPECT_THAT(local.Read(test_file), IsOkAndHolds(Eq(kTestData)));
}

TEST(RemoteHostAdapter, CreateRemoteHostAdapter) {
  EXPECT_OK(RemoteHostAdapter::Create("127.0.0.1"));
}

TEST(RemoteHostAdapter, RunCommandSuccess) {
  auto conn = std::make_unique<remote::MockConnInterface>();
  EXPECT_CALL(*conn, RunCommand)
      .WillOnce(Return(remote::ConnInterface::CommandResult{
          .exit_code = 0,
          .stdout = "stdout content",
          .stderr = "stderr content",
      }));

  RemoteHostAdapter remote(std::move(conn));

  absl::StatusOr<HostAdapter::CommandResult> result =
      remote.RunCommand(absl::Minutes(1), {"ls"});

  ASSERT_OK(result);
  EXPECT_EQ(result->exit_code, 0);
  EXPECT_EQ(result->stdout, "stdout content");
  EXPECT_EQ(result->stderr, "stderr content");
}

TEST(RemoteHostAdapter, RunCommandTimeout) {
  auto conn = std::make_unique<remote::MockConnInterface>();
  EXPECT_CALL(*conn, RunCommand)
      .WillOnce(Return(absl::DeadlineExceededError("Timeout")));

  RemoteHostAdapter remote(std::move(conn));

  EXPECT_THAT(remote.RunCommand(absl::Minutes(1), {"ls"}),
              StatusIs(absl::StatusCode::kDeadlineExceeded));
}

TEST(RemoteHostAdapter, ReadFileSuccess) {
  auto conn = std::make_unique<remote::MockConnInterface>();
  EXPECT_CALL(*conn, ReadFile(_)).WillOnce(Return(absl::Cord("content")));

  RemoteHostAdapter remote(std::move(conn));

  EXPECT_THAT(remote.Read(std::filesystem::path("filename")),
              IsOkAndHolds(Eq("content")));
}

TEST(RemoteHostAdapter, ReadFileError) {
  auto conn = std::make_unique<remote::MockConnInterface>();
  EXPECT_CALL(*conn, ReadFile(_))
      .WillOnce(Return(absl::InternalError("interal error")));

  RemoteHostAdapter remote(std::move(conn));

  EXPECT_THAT(remote.Read(std::filesystem::path("filename")),
              StatusIs(absl::StatusCode::kInternal));
}

TEST(RemoteHostAdapter, WriteFileSuccess) {
  auto conn = std::make_unique<remote::MockConnInterface>();
  EXPECT_CALL(*conn, WriteFile).WillOnce(Return(absl::OkStatus()));

  RemoteHostAdapter remote(std::move(conn));

  EXPECT_OK(remote.Write("filename", "data"));
}

TEST(RemoteHostAdapter, WriteFileError) {
  auto conn = std::make_unique<remote::MockConnInterface>();
  EXPECT_CALL(*conn, WriteFile)
      .WillOnce(Return(absl::InternalError("internal error")));

  RemoteHostAdapter remote(std::move(conn));

  EXPECT_THAT(remote.Write("filename", "data"),
              StatusIs(absl::StatusCode::kInternal));
}

TEST(HostAdapter, GetHostnameSuccess) {
  auto conn = std::make_unique<remote::MockConnInterface>();
  EXPECT_CALL(*conn, RunCommand)
      .WillOnce(Return(remote::ConnInterface::CommandResult{
          .exit_code = 0,
          // In real life stdout can end in a newline character.
          .stdout = "the_best_hostname_ever\n",
          .stderr = "stderr content",
      }));

  RemoteHostAdapter remote(std::move(conn));

  EXPECT_THAT(remote.GetHostname(), IsOkAndHolds("the_best_hostname_ever"));
}

TEST(HostAdapter, GetHostnameError) {
  auto conn = std::make_unique<remote::MockConnInterface>();
  EXPECT_CALL(*conn, RunCommand)
      .WillOnce(Return(remote::ConnInterface::CommandResult{
          .exit_code = 1,
          .stdout = "the_best_hostname_ever",
          .stderr = "stderr content",
      }));

  RemoteHostAdapter remote(std::move(conn));

  EXPECT_THAT(remote.GetHostname(absl::Seconds(10)),
              StatusIs(absl::StatusCode::kUnknown));
}

TEST(HostAdapterTest, CreateRemoteHostAdapter) {
  ASSERT_OK_AND_ASSIGN(auto remote_host, HostAdapter::Create("127.0.0.1"));
  EXPECT_NE(dynamic_cast<RemoteHostAdapter*>(remote_host.get()), nullptr);
}

TEST(HostAdapterTest, CreateLocalHostAdapter) {
  ASSERT_OK_AND_ASSIGN(auto local_host, HostAdapter::Create(""));
  EXPECT_NE(dynamic_cast<LocalHostAdapter*>(local_host.get()), nullptr);
}

}  // namespace
}  // namespace ocpdiag::hwinterface
