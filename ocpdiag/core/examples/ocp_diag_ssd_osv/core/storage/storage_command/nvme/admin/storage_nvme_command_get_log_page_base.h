// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_STORAGE_NVME_GET_LOG_PAGE_BASE_H
#define OCP_DIAG_SSD_OSV_STORAGE_NVME_GET_LOG_PAGE_BASE_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_admin_command_base.h"

namespace ocp_diag_ssd_osv {

enum class NVMeGetLogPageID : unsigned char
{
    SupportedLogPages			= 0x00,
    ErrorInfo					= 0x01,
    SMARTHealthInfo				= 0x02,
    FirmwareSlotInfo			= 0x03,
    ChangedNamespaceList		= 0x04,
    CommandsSupportAndEffets	= 0x05,
    DeviceSelfTest				= 0x06,
    TelemetryHostInitiated		= 0x07,
};

class StorageNVMeGetLogPageCommandBase : public StorageNVMeAdminCommandBase {

protected:
    enum : unsigned int
    {
        DEFAULT_BUFFER_SIZE = 4096,
    };

    enum : unsigned char
    {
        LOG_PAGE_ID_OFFSET = 0,
        LOG_PAGE_ID_LENGTH = 8
    };

public:
    explicit StorageNVMeGetLogPageCommandBase(NVMeGetLogPageID lid,
                                              size_t bufferSize = DEFAULT_BUFFER_SIZE);

    ~StorageNVMeGetLogPageCommandBase() override = default;

public:
    bool                            BeforeCommandExecutionHandler() override;
    void                            AfterCommandExecutionHandler() override;

    unsigned char                   GetLogPageId();
    void                            SetLogPageId(NVMeGetLogPageID lid);

    void                            SetLogPageOffset(unsigned long long value);
    void                            SetLogPageOffsetLower(unsigned int value);
    void                            SetLogPageOffsetUpper(unsigned int value);
    std::string                     GetCommandName() override;

protected:
    void                            SetXferSize(unsigned int xferSizeInBytes);

};

}

#endif //OCP_DIAG_SSD_OSV_STORAGE_NVME_GET_LOG_PAGE_BASE_H
