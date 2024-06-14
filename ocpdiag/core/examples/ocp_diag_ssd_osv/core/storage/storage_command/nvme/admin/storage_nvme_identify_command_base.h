// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_STORAGE_NVME_IDENTIFY_COMMAND_BASE_H
#define OCP_DIAG_SSD_OSV_STORAGE_NVME_IDENTIFY_COMMAND_BASE_H

#include <utility>
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_admin_command_base.h"

namespace ocp_diag_ssd_osv {

enum class StorageNVMeIdentifyCnsType : unsigned char
{
    Unknown = 0xFF,
    Namespace = 0x00,
    Controller = 0x01,
    ActiveNamespaceList = 0x02,
    NamespaceIdentificationDescriptorList = 0x03,
    NVMSetList = 0x04,
    AllocatedNamespaceIdList = 0x10,
    IdentifyNamespaceDataStructureForAnAllocatedNamespaceID = 0x11,
    NamespaceAttachedControllerList = 0x12,
    ControllerList = 0x13,
    PrimaryControllerCapabilitiesDataStructure = 0x14,
};

// Base class for NVMe Identify Commands.
// - For NVMe Identify Controller, NVMe Identify Namespace.
class StorageNVMeIdentifyCommandBase : public StorageNVMeAdminCommandBase {
protected:
    enum : size_t
    {
        CNS_OFFSET = 0,
        CNS_LENGTH = 8,

        BUFFER_SIZE = 4096
    };

public:
    explicit StorageNVMeIdentifyCommandBase(StorageNVMeIdentifyCnsType cns);
    ~StorageNVMeIdentifyCommandBase() override = default;

    StorageCommandType              GetStorageCommandType() override { return StorageCommandType::NVMeAdminCommand; }

    bool                            BeforeCommandExecutionHandler() override;

protected:
    void                            SetCNS(StorageNVMeIdentifyCnsType cns);
};

}

#endif //OCP_DIAG_SSD_OSV_STORAGE_NVME_IDENTIFY_COMMAND_BASE_H
