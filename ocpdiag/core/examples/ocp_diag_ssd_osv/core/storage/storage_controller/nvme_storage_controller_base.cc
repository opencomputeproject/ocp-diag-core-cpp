// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "nvme_storage_controller_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/storage_command_result/nvme/storage_nvme_command_result.h"

namespace ocp_diag_ssd_osv {

void NVMeStorageControllerBase::SendCommandImpl(RAIIHandle &handle, std::shared_ptr<StorageCommandBase> storageCommand) {
    // Is Not NVMe Command? (Is Not Instance of StorageNVMeCommandBase?)
    std::shared_ptr<StorageNVMeCommandBase> storageNVMeCommandBase = std::dynamic_pointer_cast<StorageNVMeCommandBase>(storageCommand);
    if(storageNVMeCommandBase == nullptr) {
        storageCommand->SetStorageCommandResult(std::make_shared<StorageNVMeCommandResult>(
                storageCommand->GetCommandName(),
                CommonErrorCode::NotSupported));
        return;
    }

    if(storageCommand->GetStorageCommandResult() == nullptr)
        storageCommand->SetStorageCommandResult(std::make_shared<StorageNVMeCommandResult>(storageCommand->GetCommandName()));

    SendNVMeCommand(handle, storageNVMeCommandBase);

}

void NVMeStorageControllerBase::SendNVMeCommand(RAIIHandle &handle,
                                                std::shared_ptr<StorageNVMeCommandBase> nvmeCommand) {

    // I/O Command
    {
        std::shared_ptr<StorageNVMeIoCommandBase> nvmeIoCommand = std::dynamic_pointer_cast<StorageNVMeIoCommandBase>(nvmeCommand);
        if(nvmeIoCommand) {
            SendNVMeIoCommand(handle, nvmeIoCommand);
            return;
        }
    }

    // Admin Command
    {
        std::shared_ptr<StorageNVMeAdminCommandBase> nvmeAdminCommand = std::dynamic_pointer_cast<StorageNVMeAdminCommandBase>(nvmeCommand);
        if(nvmeAdminCommand) {
            SendNVMeAdminCommand(handle, nvmeAdminCommand);
            return;
        }
    }

    nvmeCommand->SetStorageCommandResult(std::make_shared<StorageNVMeCommandResult>(nvmeCommand->GetCommandName(),
                                                                                    CommonErrorCode::NotImplemented));

}


}


