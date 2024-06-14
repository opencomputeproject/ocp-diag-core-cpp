// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_NVME_STORAGE_CONTROLLER_LINUX_INBOX_H
#define OCP_DIAG_SSD_OSV_NVME_STORAGE_CONTROLLER_LINUX_INBOX_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_controller/nvme_storage_controller_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/io/storage_nvme_io_command_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_admin_command_base.h"

namespace ocp_diag_ssd_osv {

// Class for controlling NVMe drives operating with Linux NVMe driver in-box support
class NVMeStorageControllerLinuxInbox : public NVMeStorageControllerBase {
public:
    NVMeStorageControllerLinuxInbox() : NVMeStorageControllerBase(StorageControllerType::LinuxInbox) {
    }
    ~NVMeStorageControllerLinuxInbox() override = default;

protected:
    void                            SendNVMeAdminCommand(RAIIHandle& handle,
                                                      std::shared_ptr<StorageNVMeAdminCommandBase> nvmeCommand) override;
    void                            SendNVMeIoCommand(RAIIHandle& handle,
                                                      std::shared_ptr<StorageNVMeIoCommandBase> nvmeIoCommand) override;

    void                            SendNVMeCommandImpl(RAIIHandle& handle,
                                                        std::shared_ptr<StorageNVMeCommandBase> nvmeCommand,
                                                        unsigned long request,
                                                        bool isMetadataEndOfLba = false, size_t totalMetadataSizeInBytes = 0);
};

}

#endif //OCP_DIAG_SSD_OSV_NVME_STORAGE_CONTROLLER_LINUX_INBOX_H
