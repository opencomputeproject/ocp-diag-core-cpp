// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <fcntl.h>
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_model/storage_info_base.h"

namespace ocp_diag_ssd_osv {

ocpdiag::results::HardwareInfo StorageInfoBase::To() const
{
    return {
            .name = GetModelNumber(),
            .computer_system = "",
            .location = devPath_,
            .odata_id = "",
            .part_number = "",
            .serial_number = GetSerialNumber(),
            .manager = "",
            .manufacturer = "",
            .manufacturer_part_number = "",
            .part_type = "Storage",
            .version = GetFwVersion(),
            .revision = "",
    };
}

RAIIHandle::handleValueType StorageInfoBase::CreateDeviceHandle() {
    return CreateDeviceHandle(devPath_);
}

RAIIHandle::handleValueType StorageInfoBase::CreateDeviceHandle(const std::string &devPath) {
    return ::open(devPath.c_str(), O_RDONLY | O_ASYNC /* | O_SYNC */);
}

bool StorageInfoBase::SendStorageCommand(RAIIHandle &handle,
                                         std::shared_ptr<StorageCommandBase> storageCommand) {
    return GetStorageController()->SendCommand(handle, storageCommand);
}

}


