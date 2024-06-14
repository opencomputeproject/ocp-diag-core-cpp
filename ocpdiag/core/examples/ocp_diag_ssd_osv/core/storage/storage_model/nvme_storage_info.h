// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_NVME_STORAGE_INFO_H
#define OCP_DIAG_SSD_OSV_NVME_STORAGE_INFO_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_model/storage_info_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_identify_controller_data.h"

namespace ocp_diag_ssd_osv {

class NVMeStorageInfo : public StorageInfoBase {
public:
    NVMeStorageInfo(const std::string &devPath, const std::shared_ptr<StorageControllerBase> &storageController)
            : StorageInfoBase(devPath, storageController, StorageProtocol::NVMe) {}

    ~NVMeStorageInfo() override = default;

public:
    void                                SetIdentifyControllerData(const NVMeIdentifyControllerData& identifyControllerData) { identifyControllerData_ = identifyControllerData; }

    std::string                         GetSerialNumber() const override { return identifyControllerData_.serialNumber; }
    std::string                         GetFwVersion() const override { return identifyControllerData_.fwRev; }
    std::string                         GetModelNumber() const override { return identifyControllerData_.modelNumber; }

protected:
    NVMeIdentifyControllerData          identifyControllerData_{};
};

}

#endif //OCP_DIAG_SSD_OSV_NVME_STORAGE_INFO_H
