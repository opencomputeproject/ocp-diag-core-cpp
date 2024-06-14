// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_STORAGE_FIXTURE_H
#define OCP_DIAG_SSD_OSV_STORAGE_FIXTURE_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_model/nvme_storage_info.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_controller/nvme_storage_controller_base.h"

namespace ocp_diag_ssd_osv {

// Fixture class for Unit Test
class StorageFixture {
public:
    // Create Mock NVMe Device
    static std::shared_ptr<NVMeStorageInfo>    CreateMockNVMeDevice(const std::string& devPath = "/dev/nvme0",
                                                                    StorageControllerType storageControllerType = StorageControllerType::Test);

    // Create Predefined Normal NVMe Device
    static std::shared_ptr<NVMeStorageInfo>    CreateFakeNVMeDevice(const std::string& devPath = "/dev/nvme0");
};

}

#endif // OCP_DIAG_SSD_OSV_STORAGE_FIXTURE_H
