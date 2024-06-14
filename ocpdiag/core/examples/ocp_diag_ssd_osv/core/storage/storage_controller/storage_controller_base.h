// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_STORAGE_CONTROLLER_BASE_H
#define OCP_DIAG_SSD_OSV_STORAGE_CONTROLLER_BASE_H

#include <memory>
#include <vector>

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/utils/RAIIHandle.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/storage_command_base.h"

namespace ocp_diag_ssd_osv {

enum class StorageControllerType
{
    Unknown,
    Test,
    LinuxInbox,
};

// Class for sending a storage command to storage devices driver.
class StorageControllerBase : public std::enable_shared_from_this<StorageControllerBase> {
public:
    explicit StorageControllerBase(StorageControllerType storageControllerType) : storageControllerType_(storageControllerType) {
    }

    virtual ~StorageControllerBase() = default;

public:
    StorageControllerType               GetStorageControllerType() const { return storageControllerType_; }

    bool                                SendCommand(RAIIHandle& handle,
                                                    std::shared_ptr<StorageCommandBase> storageCommand);

protected:
    virtual void                        SendCommandImpl(RAIIHandle& handle,
                                                        std::shared_ptr<StorageCommandBase> storageCommand) = 0;

private:
    StorageControllerType               storageControllerType_;
};

}

#endif //OCP_DIAG_SSD_OSV_STORAGE_CONTROLLER_BASE_H
