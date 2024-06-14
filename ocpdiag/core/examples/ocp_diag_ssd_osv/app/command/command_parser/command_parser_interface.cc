// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "command_parser_interface.h"

namespace ocp_diag_ssd_osv {

bool CommandParserInterface::ParseCommand(OcpDiagSsdTestProperties &testProperties,
                                          size_t &currentCommandIndex,
                                          const std::vector<char *> &commandList) {
    if(currentCommandIndex >= commandList.size())
        return false;

    return ParseCommandImpl(testProperties, currentCommandIndex, commandList);
}

}

