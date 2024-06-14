// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <vector>

#include "gtest/gtest.h"
#include "command_parser_test_define.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/command_parser/target_disks_command_parser.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/bad_argument_exception.h"

namespace ocp_diag_ssd_osv {

#define PREPARE_AND_EXECUTE_TARGET_DISK_COMMAND_PARSER_FOR_TEST()           \
    PREPARE_COMMAND_PARSER_FOR_TEST()                                       \
    bool ret = TargetDisksCommandParser().ParseCommand(properties, currentCommandListIndex, givenCommandList);

TEST(TargetDisksCommandParserTest, NoDiskParameter) {
    std::vector<char*> givenCommandList = {};
    PREPARE_AND_EXECUTE_TARGET_DISK_COMMAND_PARSER_FOR_TEST()

    ASSERT_FALSE(ret);
    EXPECT_TRUE(properties.targetDiskPaths.empty());
}

TEST(TargetDisksCommandParserTest, ValidDiskParameter) {
    std::vector<char*> givenCommandList = {(char*)"-d", (char*)"/dev/nvme0"};
    PREPARE_AND_EXECUTE_TARGET_DISK_COMMAND_PARSER_FOR_TEST()

    ASSERT_TRUE(ret);
    ASSERT_EQ(properties.targetDiskPaths.size(), 1);
    EXPECT_EQ(properties.targetDiskPaths[0], "/dev/nvme0");
}

TEST(TargetDisksCommandParserTest, InvalidDiskParameter) {
    std::vector<char*> givenCommandList = {(char*)"-z", (char*)"/dev/nvme0"};
    PREPARE_AND_EXECUTE_TARGET_DISK_COMMAND_PARSER_FOR_TEST()

    ASSERT_FALSE(ret);
    EXPECT_TRUE(properties.targetDiskPaths.empty());
}

TEST(TargetDisksCommandParserTest, NoDiskPathFollowingValidDiskParameter) {
    std::vector<char*> givenCommandList = {(char*)"-d"};
    PREPARE_COMMAND_PARSER_FOR_TEST()

    ASSERT_THROW(TargetDisksCommandParser().ParseCommand(properties, currentCommandListIndex, givenCommandList),
                 BadArgumentException);
}

}