// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <cstdlib>
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/ocp_diag_ssd_osv_app.h"

// Main entrypoint for ocp_diag_ssd_osv example.
int main(int argc, char* argv[]) {

    std::vector<char*> commandLine;
    for(int i = 0; i < argc; i++)
        commandLine.push_back(argv[i]);

    ocp_diag_ssd_osv::OcpDiagSsdApp().ExecuteTest(commandLine);

    return EXIT_SUCCESS;

}
