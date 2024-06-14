// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "fake_nvme_storage_controller.h"

namespace ocp_diag_ssd_osv {

#define IN_MOCK_NVME_STORAGE_CONTROLLER_BASE_SET_NVME_COMMAND_NOT_IMPLEMENTED()            \
    nvmeCommand->SetStorageCommandResult(std::make_shared<StorageNVMeCommandResult>(       \
            nvmeCommand->GetCommandName(),                                                 \
            CommonErrorCode::NotImplemented                                                \
    ))


void FakeNVMeStorageController::SendNVMeAdminCommand(RAIIHandle &handle,
                                                     std::shared_ptr<StorageNVMeAdminCommandBase> nvmeCommand) {

    switch(nvmeCommand->GetNVMeAdminCommandOpCode())
    {
        case NVMeAdminCommandOpCode::Identify:
            SendIdentifyCommand(handle, nvmeCommand);
            break;
        case NVMeAdminCommandOpCode::GetLogPage:
            SendGetLogPageCommand(handle, nvmeCommand);
            break;
        default:
            IN_MOCK_NVME_STORAGE_CONTROLLER_BASE_SET_NVME_COMMAND_NOT_IMPLEMENTED();
            break;
    }

}

void FakeNVMeStorageController::SendNVMeIoCommand(RAIIHandle &handle,
                                                  std::shared_ptr<StorageNVMeIoCommandBase> nvmeIoCommand) {
    // Nothing to do
}

void FakeNVMeStorageController::SendIdentifyCommand(RAIIHandle &handle,
                                                    std::shared_ptr<StorageNVMeCommandBase> nvmeCommand) {

    // Identify Controller
    {
        std::shared_ptr<StorageNVMeIdentifyControllerCommand> identifyControllerCommand =
                std::dynamic_pointer_cast<StorageNVMeIdentifyControllerCommand>(nvmeCommand);

        if(identifyControllerCommand) {
            SendIdentifyControllerCommand(handle, identifyControllerCommand);
            return;
        }
    }

    // Identify Namespace
    {
        std::shared_ptr<StorageNVMeIdentifyNamespaceCommand> identifyNamespaceCommand =
                std::dynamic_pointer_cast<StorageNVMeIdentifyNamespaceCommand>(nvmeCommand);

        if(identifyNamespaceCommand) {
            SendIdentifyNamespaceCommand(handle, identifyNamespaceCommand);
            return;
        }
    }

    IN_MOCK_NVME_STORAGE_CONTROLLER_BASE_SET_NVME_COMMAND_NOT_IMPLEMENTED();

}

void FakeNVMeStorageController::SendGetLogPageCommand(RAIIHandle &handle,
                                                      std::shared_ptr<StorageNVMeCommandBase> nvmeCommand) {


    // Get Log Page(SMART Health Info)
    {
        std::shared_ptr<StorageNVMeGetLogPageSmartHealthInfoCommand> getLogPageSmartHealthInfoCommand =
                std::dynamic_pointer_cast<StorageNVMeGetLogPageSmartHealthInfoCommand>(nvmeCommand);

        if(getLogPageSmartHealthInfoCommand) {
            SendGetLogPageSmartHealthInfoCommand(handle, getLogPageSmartHealthInfoCommand);
            return;
        }
    }

    // Get Log Page(Default)
    {
        std::shared_ptr<StorageNVMeGetLogPageCommandBase> nvmeGetLogPageCommandBase =
                std::dynamic_pointer_cast<StorageNVMeGetLogPageCommandBase>(nvmeCommand);

        if(nvmeGetLogPageCommandBase) {
            SendOtherGetLogPageCommand(handle, nvmeGetLogPageCommandBase);
            return;
        }
    }

    IN_MOCK_NVME_STORAGE_CONTROLLER_BASE_SET_NVME_COMMAND_NOT_IMPLEMENTED();

}

void FakeNVMeStorageController::SendIdentifyControllerCommand(RAIIHandle &handle,
                                                              std::shared_ptr<StorageNVMeIdentifyControllerCommand> nvmeIdentifyControllerCommand) {
    std::vector<unsigned char>& dataBufferRef = nvmeIdentifyControllerCommand->GetDataBufferRef();
    if(dataBufferRef.size() != 4096)
        dataBufferRef.assign(4096, 0);

    // Fake Serial Number
    std::string fakeSerialNumber = "THISISFAKESERIAL    ";
    for (size_t i = 0; i < fakeSerialNumber.size(); ++i)
        dataBufferRef[NVMeIdentifyControllerData::SN_OFFSET + i] = fakeSerialNumber[i];

    // Fake Model Number
    std::string fakeModelNumber = "THIS-IS-FAKE-SSD-MODEL";
    for (size_t i = 0; i < fakeModelNumber.size(); ++i)
        dataBufferRef[NVMeIdentifyControllerData::MODEL_OFFSET + i] = fakeModelNumber[i];

    // Fake FW Rev
    std::string fakeFwRev = "FAKEFW00";
    for (size_t i = 0; i < fakeFwRev.size(); ++i)
        dataBufferRef[NVMeIdentifyControllerData::FW_OFFSET + i] = fakeFwRev[i];

    // Number of namespace = 1
    dataBufferRef[516] = 1;

}

void FakeNVMeStorageController::SendIdentifyNamespaceCommand(RAIIHandle &handle,
                                                             std::shared_ptr<StorageNVMeIdentifyNamespaceCommand> nvmeIdentifyNamespaceCommand) {
    std::vector<unsigned char>& dataBufferRef = nvmeIdentifyNamespaceCommand->GetDataBufferRef();
    if(dataBufferRef.size() != 4096)
        dataBufferRef.assign(4096, 0);

    // NVMe Capacity
    dataBufferRef[8] = 0x00;
    dataBufferRef[9] = 0x00;
    dataBufferRef[10] = 0x80;
    dataBufferRef[11] = 0x00;
    dataBufferRef[12] = 0x00;
    dataBufferRef[13] = 0x00;
    dataBufferRef[14] = 0x00;
    dataBufferRef[15] = 0x00;

    // LBA Format Namespace Supports : lbaDataSize(PowerOfTwo) = 9
    dataBufferRef[130] = 9;
}

void FakeNVMeStorageController::SendGetLogPageSmartHealthInfoCommand(RAIIHandle &handle,
                                                                     std::shared_ptr<StorageNVMeGetLogPageSmartHealthInfoCommand> nvmeGetLogPageSmartHealthInfoCommand) {
    std::vector<unsigned char>& dataBufferRef = nvmeGetLogPageSmartHealthInfoCommand->GetDataBufferRef();
    if(dataBufferRef.size() != 4096)
        dataBufferRef.assign(4096, 0);

    // Available Spare
    dataBufferRef[3] = 100;
}

void FakeNVMeStorageController::SendOtherGetLogPageCommand(RAIIHandle &handle,
                                                           std::shared_ptr<StorageNVMeGetLogPageCommandBase> nvmeGetLogPageCommandBase) {
    // Nothing to do
}

}