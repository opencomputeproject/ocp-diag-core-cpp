// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/storage_nvme_command_base.h"

namespace ocp_diag_ssd_osv {

StorageNVMeCommandBase::StorageNVMeCommandBase(unsigned char opCode) {
    submissionQueueData_ = std::vector<unsigned int>(SUBMISSION_QUEUE_DW_SIZE, 0);
    SetOpCode(opCode);
}

StorageNVMeCommandBase::StorageNVMeCommandBase(NVMeAdminCommandOpCode opCode) :
    StorageNVMeCommandBase(static_cast<unsigned char>(opCode)) {
}

StorageNVMeCommandBase::StorageNVMeCommandBase(NVMeCommandOpCode opCode) :
    StorageNVMeCommandBase(static_cast<unsigned char>(opCode)) {
}

std::shared_ptr<StorageNVMeCommandResult> StorageNVMeCommandBase::GetStorageNVMeCommandResult() {
    return std::dynamic_pointer_cast<StorageNVMeCommandResult>(commandResult_);
}

void StorageNVMeCommandBase::ClearSubmissionQueueBit(const size_t dwOffset, const size_t bitOffset,
                                                     const size_t bitLength) {

    unsigned int bitToClear = 0;
    for (size_t i = bitOffset; i < (bitOffset + bitLength); ++i)
    {
        bitToClear |= (1 << i);
    }

    submissionQueueData_[dwOffset] &= ~bitToClear;
}

void StorageNVMeCommandBase::SetNamespaceId(unsigned int namespaceId) {
    submissionQueueData_[NAMESPACE_OFFSET] = namespaceId;
}

void StorageNVMeCommandBase::SetOpCode(unsigned char OpCode)
{
    ClearSubmissionQueueBit(CDW0_OFFSET, OPCODE_OFFSET, OPCODE_LENGTH);
    submissionQueueData_[CDW0_OFFSET] |= (static_cast<unsigned int>(OpCode) << OPCODE_OFFSET);
}


}



