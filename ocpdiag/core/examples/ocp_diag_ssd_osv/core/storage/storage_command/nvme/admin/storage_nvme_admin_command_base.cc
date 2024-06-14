// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_admin_command_base.h"

namespace ocp_diag_ssd_osv {

StorageNVMeAdminCommandBase::StorageNVMeAdminCommandBase(NVMeAdminCommandOpCode opCode)
    : StorageNVMeCommandBase(opCode) {
}

NVMeAdminCommandOpCode StorageNVMeAdminCommandBase::GetNVMeAdminCommandOpCode() const {
    return static_cast<NVMeAdminCommandOpCode>(submissionQueueData_[CDW0_OFFSET]);
}

}


