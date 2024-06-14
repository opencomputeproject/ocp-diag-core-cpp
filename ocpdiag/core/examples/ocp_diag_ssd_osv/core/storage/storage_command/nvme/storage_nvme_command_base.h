// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_CORE_STORAGE_NVME_COMMAND_BASE_H
#define OCP_DIAG_SSD_OSV_CORE_STORAGE_NVME_COMMAND_BASE_H

#include <utility>

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/storage_command_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/storage_command_result/nvme/storage_nvme_command_result.h"

namespace ocp_diag_ssd_osv {

enum class NVMeAdminCommandOpCode : unsigned char
{
    DeleteSubmissionQueue = 0x00,
    CreateSubmissionQueue = 0x01,
    GetLogPage = 0x02,
    DeleteCompletionQueue = 0x04,
    CreateCompletionQueue = 0x05,
    Identify = 0x06,
    Abort = 0x08,
    SetFeatures = 0x09,
    GetFeatures = 0x0A,
    AsyncEventRequest = 0x0C,
    NamespaceManagement = 0x0D,
    FirmwareCommit = 0x10,
    FirmwareImageDownload = 0x11,
    DeviceSelfTest = 0x14,
    NamespaceAttachment = 0x15,

    FormatNVM = 0x80,
    SecuritySend = 0x81,
    SecurityReceive = 0x82,
};

enum class NVMeCommandOpCode : unsigned char
{
    Flush = 0x00,
    Write = 0x01,
    Read = 0x02,
    WriteUncorrectable = 0x04,
    Compare = 0x05,
    WriteZeroes = 0x08,
    DataSetManagement = 0x09,
    ReservationRegister = 0x0D,
    ReservationReport = 0x0E,
    ReservationAcquire = 0x11,
    ReservationRelease = 0x15,
};

class StorageNVMeCommandBase : public StorageCommandBase {
protected:
    enum : unsigned int
    {
        SUBMISSION_QUEUE_DW_SIZE = 16,
        COMPLETION_QUEUE_DW_SIZE = 4,

        CDW0_OFFSET = 0,
        OPCODE_OFFSET = 0,
        OPCODE_LENGTH = 8,
        NAMESPACE_OFFSET = 1,
        METADATA_OFFSET = 4,
        PRP_ENTRY1_OFFSET = 6,
        PRP_ENTRY2_OFFSET = 8,
        CDW9_OFFSET = 9,
        CDW10_OFFSET = 10,
        CDW11_OFFSET = 11,
        CDW12_OFFSET = 12,
        CDW13_OFFSET = 13,
        CDW14_OFFSET = 14,
        CDW15_OFFSET = 15,
    };

public:
    explicit StorageNVMeCommandBase(unsigned char opCode);
    explicit StorageNVMeCommandBase(NVMeAdminCommandOpCode opCode);
    explicit StorageNVMeCommandBase(NVMeCommandOpCode opCode);
    ~StorageNVMeCommandBase() override = default;

public:
    void                                        SetNamespaceId(unsigned int namespaceId);
    unsigned int						        GetNamespaceId() const { return submissionQueueData_[NAMESPACE_OFFSET]; }
    std::shared_ptr<StorageNVMeCommandResult>	GetStorageNVMeCommandResult();
    const std::vector<unsigned int>&			GetSubmissionQueueDataRef() { return submissionQueueData_; }

protected:
    void                                        SetOpCode(unsigned char OpCode);
    void                                        ClearSubmissionQueueBit(const size_t dwOffset, const size_t bitOffset, const size_t bitLength);

protected:
    std::vector<unsigned int>			        submissionQueueData_;
};

}

#endif //OCP_DIAG_SSD_OSV_CORE_STORAGE_NVME_COMMAND_BASE_H
