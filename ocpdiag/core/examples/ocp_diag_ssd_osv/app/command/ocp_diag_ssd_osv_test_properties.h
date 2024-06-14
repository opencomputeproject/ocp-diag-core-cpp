// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_TEST_PROPERTIES_H
#define OCP_DIAG_SSD_OSV_TEST_PROPERTIES_H

#include <vector>
#include <string>

namespace ocp_diag_ssd_osv {

enum class TestType {
    Unknown,
    Initial,    // Initial(Incoming) Test
    Field,      // Field Test
};

struct OcpDiagSsdTestProperties {
    std::vector <std::string>   targetDiskPaths;                // empty = all disks
    TestType                    testType = TestType::Unknown;
};

}

#endif //ocp_diag_ssd_osv_TEST_PROPERTIES_H
