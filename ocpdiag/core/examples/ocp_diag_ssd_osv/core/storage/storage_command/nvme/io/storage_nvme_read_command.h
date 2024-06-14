// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_STORAGE_NVME_READ_COMMAND_H
#define OCP_DIAG_SSD_OSV_STORAGE_NVME_READ_COMMAND_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/io/storage_nvme_io_command_base.h"

namespace ocp_diag_ssd_osv {

class StorageNVMeReadCommand : public StorageNVMeIoCommandBase {
public:
    StorageNVMeReadCommand(const NVMeIdentifyNamespaceData& nvmeIdentifyNamespaceData,
                           unsigned long long startLBA,
                           unsigned int blockCount);

    ~StorageNVMeReadCommand() override = default;

protected:
    bool                        IsWriteCommand() override { return false; }

public:
    std::string                 GetCommandName() override;
};

}

#endif //OCP_DIAG_SSD_OSV_STORAGE_NVME_READ_COMMAND_H
