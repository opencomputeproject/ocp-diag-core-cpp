// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <cstring>
#include "target_disks_command_parser.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/bad_argument_exception.h"

namespace ocp_diag_ssd_osv {

bool TargetDisksCommandParser::ParseCommandImpl(OcpDiagSsdTestProperties &testProperties,
                                                size_t& currentCommandIndex,
                                                const std::vector<char *> &commandList) {

    if(strcmp(commandList[currentCommandIndex], "-d") == 0 || strcmp(commandList[currentCommandIndex], "--disk") == 0) {
        ++currentCommandIndex;
        if(commandList.size() <= currentCommandIndex) {
            throw BadArgumentException("-d or --disk option requires an target disk argument(ex: /dev/nvme0)");
        }

        testProperties.targetDiskPaths.emplace_back(commandList[currentCommandIndex]);
        return true;
    }

    return false;
}

}

