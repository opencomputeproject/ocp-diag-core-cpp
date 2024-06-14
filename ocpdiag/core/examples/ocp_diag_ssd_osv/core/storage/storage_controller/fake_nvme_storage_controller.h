// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_FAKE_NVME_STORAGE_CONTROLLER_H
#define OCP_DIAG_SSD_OSV_FAKE_NVME_STORAGE_CONTROLLER_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_controller/nvme_storage_controller_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_command_identify_controller.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_command_identify_namespace.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_command_get_log_page_smart_health_info.h"

namespace ocp_diag_ssd_osv {

//================================
// FakeNVMeStorageController
//================================

// Fake NVMe Storage Controller for Test.
// It performs a predefined action for each NVMe command. It works as a normal disk.
// If you want to control the response to each command in detail, use MockNVMeStorageController instead.
class FakeNVMeStorageController : public NVMeStorageControllerBase {
public:
    explicit FakeNVMeStorageController(StorageControllerType storageControllerType = StorageControllerType::Test)
        : NVMeStorageControllerBase(storageControllerType) {
    }
    ~FakeNVMeStorageController() override = default;

public:
    // Defines predefined behavior for each NVMe command.
    virtual void                                SendIdentifyControllerCommand(RAIIHandle &handle, std::shared_ptr<StorageNVMeIdentifyControllerCommand> nvmeIdentifyControllerCommand);
    virtual void                                SendIdentifyNamespaceCommand(RAIIHandle &handle, std::shared_ptr<StorageNVMeIdentifyNamespaceCommand> nvmeIdentifyNamespaceCommand);
    virtual void                                SendGetLogPageSmartHealthInfoCommand(RAIIHandle &handle, std::shared_ptr<StorageNVMeGetLogPageSmartHealthInfoCommand> nvmeGetLogPageSmartHealthInfoCommand);
    virtual void                                SendOtherGetLogPageCommand(RAIIHandle &handle, std::shared_ptr<StorageNVMeGetLogPageCommandBase> nvmeGetLogPageCommandBase);

protected:
    void                                        SendNVMeAdminCommand(RAIIHandle &handle, std::shared_ptr<StorageNVMeAdminCommandBase> nvmeCommand) override;
    void                                        SendNVMeIoCommand(RAIIHandle &handle, std::shared_ptr<StorageNVMeIoCommandBase> nvmeIoCommand) override;

protected:
    void                                        SendIdentifyCommand(RAIIHandle &handle, std::shared_ptr<StorageNVMeCommandBase> nvmeCommand);
    void                                        SendGetLogPageCommand(RAIIHandle &handle, std::shared_ptr<StorageNVMeCommandBase> nvmeCommand);

protected:
};

}

#endif //OCP_DIAG_SSD_OSV_FAKE_NVME_STORAGE_CONTROLLER_H
