// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <cstring>
#include "test_type_command_parser.h"

namespace ocp_diag_ssd_osv {

bool TestTypeCommandParser::ParseCommandImpl(OcpDiagSsdTestProperties &testProperties,
                                             size_t &currentCommandIndex,
                                             const std::vector<char *> &commandList) {


    char* command = commandList[currentCommandIndex];

    if(strcmp(command, "-i") == 0 || strcmp(command, "--initial") == 0) {
        testProperties.testType = TestType::Initial;
        return true;
    }

    if(strcmp(command, "-f") == 0 || strcmp(command, "--field") == 0) {
        testProperties.testType = TestType::Field;
        return true;
    }

    return false;
}

}
