// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_TEST_ARGUMENT_GENERATOR_INTERFACE_H
#define OCP_DIAG_SSD_OSV_TEST_ARGUMENT_GENERATOR_INTERFACE_H

#include <vector>
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/ocp_diag_ssd_osv_test_properties.h"

namespace ocp_diag_ssd_osv {

class CommandParserInterface {
public:
    virtual ~CommandParserInterface() = default;

public:
    bool                        ParseCommand(OcpDiagSsdTestProperties &testProperties,
                                             size_t& currentCommandIndex,
                                             const std::vector<char *> &commandList);
protected:
    virtual bool                ParseCommandImpl(OcpDiagSsdTestProperties &testProperties,
                                                 size_t& currentCommandIndex,
                                                 const std::vector<char *> &commandList) = 0;

};

}


#endif //OCP_DIAG_SSD_OSV_TEST_ARGUMENT_GENERATOR_INTERFACE_H
