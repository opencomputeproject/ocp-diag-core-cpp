// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <vector>

#include "gtest/gtest.h"
#include "command_parser/command_parser_test_define.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/command_parser_manager.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/command_parser/target_disks_command_parser.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/command_parser/test_type_command_parser.h"

namespace ocp_diag_ssd_osv {

struct CommandParserManagerTest : public testing::TestWithParam<std::vector<char*>> {};

TEST(CommandParserManagerTest, CommandParserManagerTestWithNoParameters)
{
    std::vector<char*> givenCommandList = {(char*)"ocp-diag-ssd"};
    OcpDiagSsdTestProperties testProperties = CommandParserManager({
                                                                           std::make_shared<TargetDisksCommandParser>(),
                                                                           std::make_shared<TestTypeCommandParser>(),
                                                                   }).ParseProperties(givenCommandList);

    EXPECT_EQ(testProperties.testType, TestType::Unknown);
    EXPECT_EQ(testProperties.targetDiskPaths.size(), 0);
}

TEST_P(CommandParserManagerTest, CommandParserManagerTestWithMultipleParameters)
{
    ParamType givenCommandList = CommandParserManagerTest::GetParam();
    OcpDiagSsdTestProperties testProperties = CommandParserManager({
                                                                           std::make_shared<TargetDisksCommandParser>(),
                                                                           std::make_shared<TestTypeCommandParser>(),
                                                                   }).ParseProperties(givenCommandList);

    EXPECT_EQ(testProperties.testType, TestType::Field);
    EXPECT_EQ(testProperties.targetDiskPaths.size(), 2);
    EXPECT_EQ(testProperties.targetDiskPaths[0], "/dev/nvme0");
    EXPECT_EQ(testProperties.targetDiskPaths[1], "/dev/nvme1");
}

INSTANTIATE_TEST_SUITE_P(CommandParserManagerTestWithMultipleParameterInstance,
                         CommandParserManagerTest,
                        ::testing::Values(
                                std::vector<char*>{(char*)"ocp-diag-ssd", (char*)"-d", (char*)"/dev/nvme0", (char*)"--field", (char*)"-d", (char*)"/dev/nvme1"},
                                std::vector<char*>{(char*)"ocp-diag-ssd", (char*)"-d", (char*)"/dev/nvme0", (char*)"-d", (char*)"/dev/nvme1", (char*)"--field"},
                                std::vector<char*>{(char*)"ocp-diag-ssd", (char*)"--initial", (char*)"-d", (char*)"/dev/nvme0", (char*)"-f", (char*)"-d", (char*)"/dev/nvme1"}
                        ));

}

