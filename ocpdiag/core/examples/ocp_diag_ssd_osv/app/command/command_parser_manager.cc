// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "command_parser_manager.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/bad_argument_exception.h"

ocp_diag_ssd_osv::OcpDiagSsdTestProperties ocp_diag_ssd_osv::CommandParserManager::ParseProperties(const std::vector<char*>& commandList) {

    OcpDiagSsdTestProperties result{};

    size_t currentCommandIndex = 1;
    while(currentCommandIndex < commandList.size()) {

        bool isCurrentCommandProcessed = false;
        size_t prevCommandIndex = currentCommandIndex;

        for (auto &commandParser: commandParsers_) {
            if(commandParser->ParseCommand(result, currentCommandIndex, commandList)) {
                isCurrentCommandProcessed = true;
                break;
            }
        }

        if(!isCurrentCommandProcessed)
            throw BadArgumentException(std::string("Unrecognized argument : ") + commandList[prevCommandIndex]);

        ++currentCommandIndex;
    }

    return result;
}
