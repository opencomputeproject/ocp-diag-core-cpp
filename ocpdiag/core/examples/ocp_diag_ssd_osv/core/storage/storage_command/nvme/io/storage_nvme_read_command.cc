// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/io/storage_nvme_read_command.h"

#include <utility>
#include <sstream>

namespace ocp_diag_ssd_osv {

StorageNVMeReadCommand::StorageNVMeReadCommand(const NVMeIdentifyNamespaceData& nvmeIdentifyNamespaceData,
                                               unsigned long long startLBA,
                                               unsigned int blockCount)
        : StorageNVMeIoCommandBase(NVMeCommandOpCode::Read, nvmeIdentifyNamespaceData, startLBA, blockCount) {
}

std::string StorageNVMeReadCommand::GetCommandName() {
    std::stringstream ss;
    ss << "Read(LBA=" << GetStartLBA() << ", Block Count=" << GetTargetIoBlockCount() << ")";
    return ss.str();
}

}

