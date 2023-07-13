// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/backends/lib/fake_host_adapter.h"

#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/time/time.h"
#include "absl/types/span.h"
#include "ocpdiag/core/testing/status_matchers.h"

namespace ocpdiag::hwinterface {
namespace {

using ::testing::ElementsAre;

constexpr absl::string_view kPath = "/proc/sys/file";

TEST(FakeHostAdapterTest, NominalReadWrite) {
  FakeHostAdapter fake;
  EXPECT_EQ(fake.Read(kPath).status().code(), absl::StatusCode::kNotFound);

  ASSERT_OK(fake.Write(kPath, "HELLO WORLD"));
  ASSERT_OK_AND_ASSIGN(std::string result, fake.Read(kPath));
  EXPECT_EQ(result, "HELLO WORLD");

  // Check that we can update the kPath.
  ASSERT_OK(fake.Write(kPath, "UPDATED"));
  ASSERT_OK_AND_ASSIGN(result, fake.Read(kPath));
  EXPECT_EQ(result, "UPDATED");
}

TEST(FakeHostAdapterTest, ReadError) {
  FakeHostAdapter fake;
  fake.SetReadError(kPath, absl::StatusCode::kUnknown);
  EXPECT_FALSE(fake.Read(kPath).ok());
}

TEST(FakeHostAdapterTest, WriteError) {
  FakeHostAdapter fake;
  fake.SetWriteError(kPath, absl::StatusCode::kUnknown);
  EXPECT_FALSE(fake.Write(kPath, "").ok());
}

TEST(FakeHostAdapterTest, RunCommand) {
  FakeHostAdapter fake;
  std::vector<std::string> got_args;
  absl::Duration got_timeout;

  fake.SetRunCommandCallback(
      [&](absl::Duration timeout, const std::vector<std::string>& args) {
        for (std::string_view arg : args) got_args.push_back(std::string(arg));
        got_timeout = timeout;
        return HostAdapter::CommandResult{.exit_code = 0};
      });

  absl::Duration timeout = absl::Seconds(1);
  ASSERT_OK(fake.RunCommand(timeout, {"Hello"}));

  EXPECT_THAT(got_args, ElementsAre("Hello"));
  EXPECT_EQ(got_timeout, timeout);
}

TEST(FakeHostAdapterTest, RunCommandError) {
  FakeHostAdapter fake;
  fake.SetRunCommandCallback(
      [&](auto, auto) { return absl::StatusCode::kUnknown; });

  EXPECT_FALSE(fake.RunCommand(absl::InfiniteDuration(), {"Hello"}).ok());
}

}  // namespace
}  // namespace ocpdiag::hwinterface
