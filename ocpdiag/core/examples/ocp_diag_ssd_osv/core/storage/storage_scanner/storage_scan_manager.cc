// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "storage_scan_manager.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_command_identify_controller.h"

namespace ocp_diag_ssd_osv {

std::vector<std::shared_ptr<NVMeStorageInfo>> StorageScanManager::GetAllRecognizedNVMeStorageDevices() {

    std::vector<std::shared_ptr<NVMeStorageInfo>> nvmeDeviceList;

    for(std::shared_ptr<NVMeStorageScannerInterface>& nvmeStorageScanner : nvmeStorageScanners_) {

        for(const std::string& nvmeStorageDeviceCandidatePath : nvmeStorageScanner->GetAllNVMeStorageDeviceCandidates()) {
            std::shared_ptr<NVMeStorageInfo> nvmeDeviceCandidate = std::make_shared<NVMeStorageInfo>(
                    nvmeStorageDeviceCandidatePath,
                    nvmeStorageScanner->CreateNVMeStorageController()
            );

            // Create Handle
            RAIIHandle handle = nvmeDeviceCandidate->CreateDeviceHandle();

            // NVMe Identify Controller Command must be successful
            std::shared_ptr<StorageNVMeIdentifyControllerCommand> nvmeIdentifyControllerCmd = std::make_shared<StorageNVMeIdentifyControllerCommand>();
            if(!nvmeDeviceCandidate->SendStorageCommand(handle, nvmeIdentifyControllerCmd)) {
                continue;
            }

            // Update NVMeIdentifyControllerData
            nvmeDeviceCandidate->SetIdentifyControllerData(nvmeIdentifyControllerCmd->GetParsedData());
            nvmeDeviceList.emplace_back(nvmeDeviceCandidate);

        }
    }

    return nvmeDeviceList;
}

}



