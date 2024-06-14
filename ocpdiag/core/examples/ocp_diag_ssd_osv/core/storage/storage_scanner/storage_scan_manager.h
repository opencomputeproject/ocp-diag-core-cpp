// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_STORAGE_SCAN_MANAGER_H
#define OCP_DIAG_SSD_OSV_STORAGE_SCAN_MANAGER_H

#include <vector>
#include <memory>
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_model/storage_info_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_model/nvme_storage_info.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_controller/nvme_storage_controller_linux_inbox.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_scanner/nvme/nvme_storage_scanner_interface.h"

namespace ocp_diag_ssd_osv {

// Class for listing all recognized storage devices.
// NOTE : Only supports NVMe devices now
class StorageScanManager {
public:
    StorageScanManager(const std::vector<std::shared_ptr<NVMeStorageScannerInterface>>& nvmeStorageScanners)
        : nvmeStorageScanners_(nvmeStorageScanners) {
    }

    virtual ~StorageScanManager() = default;

public:
    // Returns shared_ptr of StorageInfoBase for NVMe device machine.
    // Only the devices on which Identify Controller NVMe command was successfully performed are returned.
    std::vector<std::shared_ptr<NVMeStorageInfo>>               GetAllRecognizedNVMeStorageDevices();

protected:
    std::vector<std::shared_ptr<NVMeStorageScannerInterface>>   nvmeStorageScanners_;
};

} // namespace ocp_diag_ssd_osv

#endif // OCP_DIAG_SSD_OSV_STORAGE_SCAN_MANAGER_H
