// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_command_identify_controller.h"

namespace ocp_diag_ssd_osv {

NVMeIdentifyControllerData StorageNVMeIdentifyControllerCommand::GetParsedData() {
    return NVMeIdentifyControllerData::From(dataBuffer_);
}

void StorageNVMeIdentifyControllerCommand::AfterCommandExecutionHandler() {
    if (commandResult_->IsOK() && BufferUtils::IsBufferAllZero(dataBuffer_)) {
        commandResult_->SetCommonErrorCode(CommonErrorCode::ResultIsAllZero);
    }
}

}

