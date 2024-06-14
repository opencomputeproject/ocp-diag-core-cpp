// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_ARGUMENT_PARSER_H
#define OCP_DIAG_SSD_OSV_ARGUMENT_PARSER_H

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/ocp_diag_ssd_osv_test_properties.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/command_parser/command_parser_interface.h"

namespace ocp_diag_ssd_osv {

class CommandParserManager {
public:
    explicit CommandParserManager(std::vector<std::shared_ptr<CommandParserInterface>> commandParsers)
        : commandParsers_(std::move(commandParsers)) { }

public:
    OcpDiagSsdTestProperties                                ParseProperties(const std::vector<char*>& commandList);

private:
    std::vector<std::shared_ptr<CommandParserInterface>>    commandParsers_;
};

}

#endif //ocp_diag_ssd_osv_ARGUMENT_PARSER_H
