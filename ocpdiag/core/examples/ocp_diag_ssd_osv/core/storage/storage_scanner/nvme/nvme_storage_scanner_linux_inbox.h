// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_LINUX_NVME_STORAGE_SCANNER_H
#define OCP_DIAG_SSD_OSV_LINUX_NVME_STORAGE_SCANNER_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_scanner/nvme/nvme_storage_scanner_interface.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_controller/nvme_storage_controller_linux_inbox.h"

namespace ocp_diag_ssd_osv {

// Class for enumerating NVMe drives in Linux Inbox Driver.
class NVMeStorageScannerLinuxInbox : public NVMeStorageScannerInterface {
public:
    ~NVMeStorageScannerLinuxInbox() override = default;

public:
    // Enumerates NVMe drives in Linux Inbox Driver.
    // It may not NVMe drives, so it is necessary to check whether the device responds successful status in response to NVMe Identify Controller Command.
    std::vector<std::string>                        GetAllNVMeStorageDeviceCandidates() override;

    std::shared_ptr<NVMeStorageControllerBase>      CreateNVMeStorageController() override {
        return std::make_shared<NVMeStorageControllerLinuxInbox>();
    }

protected:
    // Root path for storage device
    const char*                                     DEVICE_ROOT_DIR = "/dev/";

    // Regex expression for NVMe devices
    std::string                                     nvmeDeviceCandidateDeviceRegex_ = "nvme[0-9]+";
};

}

#endif //OCP_DIAG_SSD_OSV_LINUX_NVME_STORAGE_SCANNER_H
