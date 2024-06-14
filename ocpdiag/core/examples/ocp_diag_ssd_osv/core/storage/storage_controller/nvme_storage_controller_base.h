// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_NVME_STORAGE_CONTROLLER_BASE_H
#define OCP_DIAG_SSD_OSV_NVME_STORAGE_CONTROLLER_BASE_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_controller/storage_controller_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/storage_nvme_command_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_admin_command_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/io/storage_nvme_io_command_base.h"

namespace ocp_diag_ssd_osv {

class NVMeStorageControllerBase : public StorageControllerBase {
public:
    explicit NVMeStorageControllerBase(StorageControllerType storageControllerType) : StorageControllerBase(storageControllerType) {
    }
    ~NVMeStorageControllerBase() override = default;

public:
    void                    SendCommandImpl(RAIIHandle& handle,
                                            std::shared_ptr<StorageCommandBase> storageCommand) final;

    void                    SendNVMeCommand(RAIIHandle& handle,
                                            std::shared_ptr<StorageNVMeCommandBase> nvmeCommand);

protected:
    virtual void            SendNVMeAdminCommand(RAIIHandle& handle,
                                                 std::shared_ptr<StorageNVMeAdminCommandBase> nvmeCommand) = 0;
    virtual void            SendNVMeIoCommand(RAIIHandle& handle,
                                              std::shared_ptr<StorageNVMeIoCommandBase> nvmeIoCommand) = 0;
};

}

#endif //OCP_DIAG_SSD_OSV_NVME_STORAGE_CONTROLLER_BASE_H
