// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_controller/storage_controller_base.h"

namespace ocp_diag_ssd_osv {

bool StorageControllerBase::SendCommand(RAIIHandle &handle, std::shared_ptr<StorageCommandBase> storageCommand) {
    if(storageCommand->BeforeCommandExecutionHandler() == false)
        return false;

    SendCommandImpl(handle, storageCommand);
    storageCommand->AfterCommandExecutionHandler();
    return storageCommand->IsOK();
}

}


