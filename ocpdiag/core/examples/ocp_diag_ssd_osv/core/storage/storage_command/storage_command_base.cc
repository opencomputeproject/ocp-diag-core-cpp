// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/storage_command_base.h"

namespace ocp_diag_ssd_osv {

bool StorageCommandBase::IsOK() {
    if(commandResult_ == nullptr)
        return false;

    return commandResult_->IsOK();
}

}

