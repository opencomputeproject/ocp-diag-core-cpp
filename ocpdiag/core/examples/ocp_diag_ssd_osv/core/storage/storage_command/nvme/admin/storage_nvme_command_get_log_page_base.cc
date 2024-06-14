#include <sstream>
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_command_get_log_page_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/utils/buffer_utils.h"

namespace ocp_diag_ssd_osv {

StorageNVMeGetLogPageCommandBase::StorageNVMeGetLogPageCommandBase(
        NVMeGetLogPageID lid, size_t bufferSize)
        : StorageNVMeAdminCommandBase(NVMeAdminCommandOpCode::GetLogPage) {

    dataBuffer_ = std::vector<unsigned char>(bufferSize, 0);
    SetLogPageId(lid);
    SetNamespaceId(0xFFFFFFFF);
}

bool StorageNVMeGetLogPageCommandBase::BeforeCommandExecutionHandler() {
    if (dataBuffer_.empty())
        dataBuffer_ = std::vector<unsigned char>(DEFAULT_BUFFER_SIZE, 0);

    SetXferSize(static_cast<unsigned int>(dataBuffer_.size()));
    return true;
}

void StorageNVMeGetLogPageCommandBase::AfterCommandExecutionHandler() {
    if (commandResult_->IsOK() && BufferUtils::IsBufferAllZero(dataBuffer_))
    {
        commandResult_->SetCommonErrorCode(CommonErrorCode::ResultIsAllZero);
    }
}

void StorageNVMeGetLogPageCommandBase::SetLogPageId(NVMeGetLogPageID lid) {
    ClearSubmissionQueueBit(CDW10_OFFSET, LOG_PAGE_ID_OFFSET, LOG_PAGE_ID_LENGTH);
    submissionQueueData_[CDW10_OFFSET] |= (static_cast<unsigned int>(lid) << LOG_PAGE_ID_OFFSET);
}

void StorageNVMeGetLogPageCommandBase::SetLogPageOffset(unsigned long long int value) {
    // The meaning of the value depends on OP bits in Get Log Page. (Please refer NVMe Spec)
    SetLogPageOffsetUpper(static_cast<unsigned int>((value & 0xFFFFFFFF00000000) >> 32));
    SetLogPageOffsetLower(static_cast<unsigned int>(value & 0x00000000FFFFFFFF));
}

void StorageNVMeGetLogPageCommandBase::SetLogPageOffsetLower(unsigned int value) {
    submissionQueueData_[CDW12_OFFSET] |= value;
}

void StorageNVMeGetLogPageCommandBase::SetLogPageOffsetUpper(unsigned int value) {
    submissionQueueData_[CDW13_OFFSET] |= value;
}

void StorageNVMeGetLogPageCommandBase::SetXferSize(unsigned int xferSizeInBytes) {
    unsigned int xferSizeInDwordWithZeroBased = static_cast<unsigned int>((xferSizeInBytes / sizeof(unsigned int)) - 1);

    static const size_t NUMBER_OF_DWORDS_LOWER_OFFSET = 16;
    static const size_t NUMBER_OF_DWORDS_LOWER_LENGTH = 16;

    size_t NUMBER_OF_DWORDS_UPPER_OFFSET = 0;
    size_t NUMBER_OF_DWORDS_UPPER_LENGTH = 16;

    ClearSubmissionQueueBit(CDW10_OFFSET, NUMBER_OF_DWORDS_LOWER_OFFSET, NUMBER_OF_DWORDS_LOWER_LENGTH);
    ClearSubmissionQueueBit(CDW11_OFFSET, NUMBER_OF_DWORDS_UPPER_OFFSET, NUMBER_OF_DWORDS_UPPER_LENGTH);

    submissionQueueData_[CDW10_OFFSET] |= ((xferSizeInDwordWithZeroBased & 0x0000FFFF) << NUMBER_OF_DWORDS_LOWER_OFFSET);
    submissionQueueData_[CDW11_OFFSET] |= ((xferSizeInDwordWithZeroBased & 0xFFFF0000) << NUMBER_OF_DWORDS_UPPER_OFFSET);
}

std::string StorageNVMeGetLogPageCommandBase::GetCommandName() {
    std::stringstream ss;
    ss << "Get Log Page(LID = " << static_cast<unsigned long>(GetLogPageId()) << ")";
    return ss.str();
}

unsigned char StorageNVMeGetLogPageCommandBase::GetLogPageId() {
    return static_cast<unsigned char>((submissionQueueData_[CDW10_OFFSET] >> LOG_PAGE_ID_OFFSET));
}

}


