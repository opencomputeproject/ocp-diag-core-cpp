// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_CORE_STORAGE_NVME_COMMAND_IDENTIFY_CONTROLLER_H
#define OCP_DIAG_SSD_OSV_CORE_STORAGE_NVME_COMMAND_IDENTIFY_CONTROLLER_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_identify_command_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_identify_controller_data.h"

namespace ocp_diag_ssd_osv {

class StorageNVMeIdentifyControllerCommand : public StorageNVMeIdentifyCommandBase {
public:
    StorageNVMeIdentifyControllerCommand() :
        StorageNVMeIdentifyCommandBase(StorageNVMeIdentifyCnsType::Controller) {}

    ~StorageNVMeIdentifyControllerCommand() override = default;

public:
    void                                    AfterCommandExecutionHandler() override;

    std::string                             GetCommandName() override { return "NVMe Identify Controller"; }
    NVMeIdentifyControllerData              GetParsedData();
};

}
#endif //OCP_DIAG_SSD_OSV_CORE_STORAGE_NVME_COMMAND_IDENTIFY_CONTROLLER_H
