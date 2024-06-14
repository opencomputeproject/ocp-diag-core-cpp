// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "nvme_storage_controller_linux_inbox.h"
#include <sys/ioctl.h>
#include <linux/nvme_ioctl.h>
#include <cstring>

namespace ocp_diag_ssd_osv {

void NVMeStorageControllerLinuxInbox::SendNVMeAdminCommand(RAIIHandle &handle,
                                                           std::shared_ptr<StorageNVMeAdminCommandBase> nvmeCommand) {
    SendNVMeCommandImpl(handle, nvmeCommand, NVME_IOCTL_ADMIN_CMD);
}

void NVMeStorageControllerLinuxInbox::SendNVMeIoCommand(RAIIHandle &handle,
                                                        std::shared_ptr<StorageNVMeIoCommandBase> nvmeIoCommand) {

    bool isMetadataEndOfLba = nvmeIoCommand->IsMetadataEndOfLBA();
    size_t totalMetadataSizeInBytes = nvmeIoCommand->CalcTotalMetadataSizeInBytes();

    SendNVMeCommandImpl(handle, nvmeIoCommand, NVME_IOCTL_IO_CMD, isMetadataEndOfLba, totalMetadataSizeInBytes);
}

void NVMeStorageControllerLinuxInbox::SendNVMeCommandImpl(RAIIHandle &handle,
                                                          std::shared_ptr<StorageNVMeCommandBase> nvmeCommand,
                                                          unsigned long request,
                                                          bool isMetadataEndOfLba, size_t totalMetadataSizeInBytes) {

    std::shared_ptr<StorageNVMeCommandResult> result = nvmeCommand->GetStorageNVMeCommandResult();
    std::vector<unsigned char>& data = nvmeCommand->GetDataBufferRef();

    void* alignedDataBuffer = nullptr;
    nvme_passthru_cmd cmd = {0, };
    memcpy(&cmd, nvmeCommand->GetSubmissionQueueDataRef().data(), sizeof(unsigned int) * 16);

    cmd.timeout_ms = nvmeCommand->GetTimeoutInSecond() * 1000;

    if(nvmeCommand->HasData())
    {
        posix_memalign(&alignedDataBuffer, getpagesize(), data.size());
        memcpy(alignedDataBuffer, data.data(), data.size());

        cmd.addr = static_cast<unsigned long long>((uintptr_t) (alignedDataBuffer));
        cmd.data_len = data.size();
    }

    // handling NVMe metadata (if exists)
    if(totalMetadataSizeInBytes > 0)
    {
        unsigned long metaDataLen = 0;
        if(totalMetadataSizeInBytes != 0) {

            if(isMetadataEndOfLba) {
                metaDataLen = 0;
            }
            else {
                // In Linux, metadata with separated buffer is not supported.
                // System freezing may be occurred in old kernel version.
                result->SetCommonErrorCode(CommonErrorCode::NotSupported);
                return;
            }

        }

        cmd.metadata_len = metaDataLen;
    }

    int status = ioctl(handle, request, &cmd);

    if(alignedDataBuffer != nullptr)
    {
        memcpy(data.data(), alignedDataBuffer, data.size());
        free(alignedDataBuffer);
        alignedDataBuffer = nullptr;
    }

    if(status < 0)
    {
        result->SetIoctlErrorCode(errno);
        result->SetCommonErrorCode(CommonErrorCode::IoctlFail);
        return;
    }

    unsigned int cpl3 = (status & 0x7FF) << 17;
    result->SetStatusType(static_cast<NVMeStatusCodeType>((cpl3 >> 25) & 0x07));
    result->SetStatusCode((cpl3 >> 17) & 0xFF);
}

}

