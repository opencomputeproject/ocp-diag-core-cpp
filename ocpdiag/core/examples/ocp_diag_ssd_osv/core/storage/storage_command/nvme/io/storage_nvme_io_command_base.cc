// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/io/storage_nvme_io_command_base.h"

namespace ocp_diag_ssd_osv {

StorageNVMeIoCommandBase::StorageNVMeIoCommandBase(NVMeCommandOpCode opCode,
                                                   const NVMeIdentifyNamespaceData& nvmeIdentifyNamespaceData,
                                                   unsigned long long startLBA,
                                                   unsigned int blockCount)
        : StorageNVMeCommandBase(opCode), nvmeIdentifyNamespaceData_(nvmeIdentifyNamespaceData) {

    SetNVMeNamespaceDataInfo(nvmeIdentifyNamespaceData_);
    SetStartLBA(startLBA);
    SetTargetIoBlockCount(blockCount);
}

bool StorageNVMeIoCommandBase::BeforeCommandExecutionHandler() {

    // In Linux, metadata with separated buffer is not supported.
    // System freezing may be occurred in the certain kernel version.
    if(nvmeIdentifyNamespaceData_.GetCurrentLbaFormat().metaDataSize > 0 && IsMetadataEndOfLBA() == false) {

        commandResult_ = std::make_shared<StorageNVMeCommandResult>(GetCommandName());
        commandResult_->SetCommonErrorCode(CommonErrorCode::NotSupported);
        return false;
    }

    // Resize dataBuffer
    if(ResizeDataBufferWithGivenInfo() == false) // Invalid Data Size
    {
        commandResult_ = std::make_shared<StorageNVMeCommandResult>(GetCommandName());
        commandResult_->SetCommonErrorCode(CommonErrorCode::LogicError);
        return false;
    }

    return true;
}

bool StorageNVMeIoCommandBase::ResizeDataBufferWithGivenInfo() {
    const size_t dataSize = CalcTotalDataSizeInBytes();
    const size_t totalMetaDataBufferSize = CalcTotalMetadataSizeInBytes();

    size_t totalDataBufferSize = dataSize;

    if(nvmeIdentifyNamespaceData_.GetCurrentLbaFormat().metaDataSize > 0) {
        // NOTE : Currently, metadata with separated buffer is not supported.
        // In certain Kernel version, system freezing may be occurred.
        if(!IsMetadataEndOfLBA())
            return false;

        totalDataBufferSize += totalMetaDataBufferSize;
    }

    if(totalDataBufferSize == 0)
        return false;

    if(dataBuffer_.size() != totalDataBufferSize)
        dataBuffer_.assign(totalDataBufferSize, 0);

    return true;
}

void StorageNVMeIoCommandBase::SetStartLBA(unsigned long long int startLba) {
    submissionQueueData_[CDW10_OFFSET] = static_cast<unsigned int>(startLba) & 0xFFFFFFFF;
    submissionQueueData_[CDW11_OFFSET] = static_cast<unsigned int>(startLba >> 32) & 0xFFFFFFFF;
}

unsigned long long StorageNVMeIoCommandBase::GetStartLBA() {
    return (static_cast<unsigned long long>(submissionQueueData_[CDW11_OFFSET]) << 32) |
           static_cast<unsigned long long>(submissionQueueData_[CDW10_OFFSET]);
}

void StorageNVMeIoCommandBase::SetNVMeNamespaceDataInfo(
        const ocp_diag_ssd_osv::NVMeIdentifyNamespaceData &nvmeIdentifyNamespaceData) {
    nvmeIdentifyNamespaceData_ = nvmeIdentifyNamespaceData;
    SetNamespaceId(nvmeIdentifyNamespaceData_.namespaceId);
}

void StorageNVMeIoCommandBase::SetTargetIoBlockCount(unsigned int blockCount) {
    submissionQueueData_[CDW12_OFFSET] |= ((blockCount-1) & 0xFFFF);
}

unsigned int StorageNVMeIoCommandBase::GetTargetIoBlockCount() const {
    return (submissionQueueData_[CDW12_OFFSET] & 0xFFFF) + 1;
}

NVMeCommandOpCode StorageNVMeIoCommandBase::GetNVMeCommandOpCode() const {
    return static_cast<NVMeCommandOpCode>(submissionQueueData_[CDW0_OFFSET]);
}

}







