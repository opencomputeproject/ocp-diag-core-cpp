// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_STORAGE_NVME_ADMIN_COMMAND_BASE_H
#define OCP_DIAG_SSD_OSV_STORAGE_NVME_ADMIN_COMMAND_BASE_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/storage_nvme_command_base.h"

namespace ocp_diag_ssd_osv {

class StorageNVMeAdminCommandBase : public StorageNVMeCommandBase {

public:
    explicit StorageNVMeAdminCommandBase(NVMeAdminCommandOpCode opCode);
    ~StorageNVMeAdminCommandBase() override = default;

public:
    StorageCommandType          GetStorageCommandType() override { return StorageCommandType::NVMeAdminCommand; }
    NVMeAdminCommandOpCode      GetNVMeAdminCommandOpCode() const;
};

}

#endif //OCP_DIAG_SSD_OSV_STORAGE_NVME_ADMIN_COMMAND_BASE_H
