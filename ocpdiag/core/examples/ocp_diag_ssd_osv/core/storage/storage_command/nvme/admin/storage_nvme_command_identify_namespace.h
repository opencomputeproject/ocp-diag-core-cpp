// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_STORAGE_NVME_IDENTIFY_NAMESPACE_COMMAND_H
#define OCP_DIAG_SSD_OSV_STORAGE_NVME_IDENTIFY_NAMESPACE_COMMAND_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_identify_command_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_identify_namespace_data.h"

namespace ocp_diag_ssd_osv {

class StorageNVMeIdentifyNamespaceCommand : public StorageNVMeIdentifyCommandBase {
public:
    explicit StorageNVMeIdentifyNamespaceCommand(unsigned int namespaceId = 1);
    ~StorageNVMeIdentifyNamespaceCommand() override = default;

public:
    std::string                             GetCommandName() override;
    NVMeIdentifyNamespaceData               GetParsedData();
};

}

#endif //OCP_DIAG_SSD_OSV_STORAGE_NVME_IDENTIFY_NAMESPACE_COMMAND_H
