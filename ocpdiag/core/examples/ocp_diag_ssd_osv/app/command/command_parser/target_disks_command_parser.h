// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_TARGET_DISKS_COMMAND_PARSER_H
#define OCP_DIAG_SSD_OSV_TARGET_DISKS_COMMAND_PARSER_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/command_parser/command_parser_interface.h"

namespace ocp_diag_ssd_osv {

class TargetDisksCommandParser : public CommandParserInterface {
public:
    ~TargetDisksCommandParser() override = default;

public:
    bool                        ParseCommandImpl(OcpDiagSsdTestProperties &testProperties,
                                                 size_t& currentCommandIndex,
                                                 const std::vector<char *> &commandList) override;

};

}


#endif // OCP_DIAG_SSD_OSV_TARGET_DISKS_COMMAND_PARSER_H
