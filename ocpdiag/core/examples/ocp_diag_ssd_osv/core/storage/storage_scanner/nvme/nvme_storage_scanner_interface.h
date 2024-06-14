// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_NVME_STORAGE_SCANNER_INTERFACE_H
#define OCP_DIAG_SSD_OSV_NVME_STORAGE_SCANNER_INTERFACE_H

#include <memory>
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_controller/nvme_storage_controller_base.h"

namespace ocp_diag_ssd_osv {

// Interface for enumerating NVMe drives.
class NVMeStorageScannerInterface {
public:
    virtual ~NVMeStorageScannerInterface() = default;

public:
    // Enumerates devices presumed to be NVMe drives in Linux Inbox Driver.
    // It may not NVMe drives, so it will be checked whether the device responds successful status in response to NVMe Identify Controller Command.
    virtual std::vector<std::string>                            GetAllNVMeStorageDeviceCandidates() = 0;
    virtual std::shared_ptr<NVMeStorageControllerBase>          CreateNVMeStorageController() = 0;
};

}

#endif //OCP_DIAG_SSD_OSV_NVME_STORAGE_SCANNER_INTERFACE_H
