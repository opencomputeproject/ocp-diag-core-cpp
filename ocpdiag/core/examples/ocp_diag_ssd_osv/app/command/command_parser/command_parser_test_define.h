// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_COMMAND_PARSER_TEST_DEFINE_H
#define OCP_DIAG_SSD_OSV_COMMAND_PARSER_TEST_DEFINE_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/ocp_diag_ssd_osv_test_properties.h"

namespace ocp_diag_ssd_osv {

#define PREPARE_COMMAND_PARSER_FOR_TEST()                       \
    OcpDiagSsdTestProperties properties{};                      \
    size_t currentCommandListIndex = 0;
}

#endif //OCP_DIAG_SSD_OSV_COMMAND_PARSER_TEST_DEFINE_H
