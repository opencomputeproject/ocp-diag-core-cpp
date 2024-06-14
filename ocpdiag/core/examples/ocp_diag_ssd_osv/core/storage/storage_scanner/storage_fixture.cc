// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "storage_fixture.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_model/nvme_storage_info.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_controller/mock_nvme_storage_controller.h"

namespace ocp_diag_ssd_osv {

std::shared_ptr<NVMeStorageInfo> StorageFixture::CreateMockNVMeDevice(const std::string& devPath, StorageControllerType storageControllerType) {
    return std::make_shared<NVMeStorageInfo>(devPath, std::make_shared<MockNVMeStorageController>(storageControllerType));
}

std::shared_ptr<NVMeStorageInfo> StorageFixture::CreateFakeNVMeDevice(const std::string& devPath) {
    return std::make_shared<NVMeStorageInfo>(devPath, std::make_shared<FakeNVMeStorageController>());
}

}

