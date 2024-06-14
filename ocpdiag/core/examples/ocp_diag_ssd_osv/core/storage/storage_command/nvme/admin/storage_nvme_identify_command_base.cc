// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_identify_command_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/utils/buffer_utils.h"

namespace ocp_diag_ssd_osv {

StorageNVMeIdentifyCommandBase::StorageNVMeIdentifyCommandBase(StorageNVMeIdentifyCnsType cns)
: StorageNVMeAdminCommandBase(NVMeAdminCommandOpCode::Identify) {
    SetCNS(cns);
}

bool StorageNVMeIdentifyCommandBase::BeforeCommandExecutionHandler() {
    if (dataBuffer_.size() < BUFFER_SIZE)
        dataBuffer_ = std::vector<unsigned char>(BUFFER_SIZE, 0);

    return true;
}

void StorageNVMeIdentifyCommandBase::SetCNS(StorageNVMeIdentifyCnsType cns) {
    ClearSubmissionQueueBit(CDW10_OFFSET, CNS_OFFSET, CNS_LENGTH);
    submissionQueueData_[CDW10_OFFSET] |= (static_cast<unsigned int>(cns) << CNS_OFFSET);
}

}



