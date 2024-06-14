// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <vector>

#include "gtest/gtest.h"
#include "command_parser_test_define.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/command_parser/test_type_command_parser.h"

namespace ocp_diag_ssd_osv {

#define PREPARE_AND_EXECUTE_TEST_TYPE_COMMAND_PARSER_FOR_TEST()           \
    PREPARE_COMMAND_PARSER_FOR_TEST()                                     \
    bool ret = TestTypeCommandParser().ParseCommand(properties, currentCommandListIndex, givenCommandList);

TEST(TestTypeCommandParserTest, NoTestTypeParameter) {
    std::vector<char*> givenCommandList = {(char*)"ocp-diag-ssd"};
    PREPARE_AND_EXECUTE_TEST_TYPE_COMMAND_PARSER_FOR_TEST()

    ASSERT_FALSE(ret);
    ASSERT_EQ(properties.testType, TestType::Unknown);
}

TEST(TestTypeCommandParserTest, WithInitialTestTypeParameter_1) {
    std::vector<char*> givenCommandList = {(char*)"-i"};
    PREPARE_AND_EXECUTE_TEST_TYPE_COMMAND_PARSER_FOR_TEST()

    ASSERT_TRUE(ret);
    ASSERT_EQ(properties.testType, TestType::Initial);
}

TEST(TestTypeCommandParserTest, WithInitialTestTypeParameter_2) {
    std::vector<char*> givenCommandList = {(char*)"--initial"};
    PREPARE_AND_EXECUTE_TEST_TYPE_COMMAND_PARSER_FOR_TEST()

    ASSERT_TRUE(ret);
    ASSERT_EQ(properties.testType, TestType::Initial);
}

TEST(TestTypeCommandParserTest, WithFieldTestTypeParameter_1) {
    std::vector<char*> givenCommandList = {(char*)"-f"};
    PREPARE_AND_EXECUTE_TEST_TYPE_COMMAND_PARSER_FOR_TEST()

    ASSERT_TRUE(ret);
    ASSERT_EQ(properties.testType, TestType::Field);
}

TEST(TestTypeCommandParserTest, WithFieldTestTypeParameter_2) {
    std::vector<char*> givenCommandList = {(char*)"--field"};
    PREPARE_AND_EXECUTE_TEST_TYPE_COMMAND_PARSER_FOR_TEST()

    ASSERT_TRUE(ret);
    ASSERT_EQ(properties.testType, TestType::Field);
}

}