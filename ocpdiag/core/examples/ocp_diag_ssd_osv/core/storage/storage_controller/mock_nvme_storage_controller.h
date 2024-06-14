// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_MOCK_NVME_STORAGE_CONTROLLER_H
#define OCP_DIAG_SSD_OSV_MOCK_NVME_STORAGE_CONTROLLER_H

#include "gmock/gmock.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_controller/fake_nvme_storage_controller.h"

namespace ocp_diag_ssd_osv {

//================================
// MockNVMeStorageController
//================================
class MockNVMeStorageController : public FakeNVMeStorageController {
public:
    explicit MockNVMeStorageController(StorageControllerType storageControllerType)
        : FakeNVMeStorageController(storageControllerType) {
    }
    ~MockNVMeStorageController() override = default;

public:
    MOCK_METHOD(void, SendIdentifyControllerCommand, (RAIIHandle &handle, std::shared_ptr<StorageNVMeIdentifyControllerCommand> nvmeIdentifyControllerCommand), (override));
    MOCK_METHOD(void, SendIdentifyNamespaceCommand, (RAIIHandle &handle, std::shared_ptr<StorageNVMeIdentifyNamespaceCommand> nvmeIdentifyNamespaceCommand), (override));
    MOCK_METHOD(void, SendGetLogPageSmartHealthInfoCommand, (RAIIHandle &handle, std::shared_ptr<StorageNVMeGetLogPageSmartHealthInfoCommand> nvmeGetLogPageSmartHealthInfoCommand), (override));
    MOCK_METHOD(void, SendOtherGetLogPageCommand, (RAIIHandle &handle, std::shared_ptr<StorageNVMeGetLogPageCommandBase> nvmeGetLogPageCommandBase), (override));
};

}

#endif //OCP_DIAG_SSD_OSV_MOCK_NVME_STORAGE_CONTROLLER_H
