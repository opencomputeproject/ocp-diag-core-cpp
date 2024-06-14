// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_STORAGE_NVME_COMMAND_GET_LOG_PAGE_SMART_HEALTH_INFO_H
#define OCP_DIAG_SSD_OSV_STORAGE_NVME_COMMAND_GET_LOG_PAGE_SMART_HEALTH_INFO_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_command_get_log_page_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_command_get_log_page_smart_health_info_data.h"

namespace ocp_diag_ssd_osv {

class StorageNVMeGetLogPageSmartHealthInfoCommand : public StorageNVMeGetLogPageCommandBase {
public:
    StorageNVMeGetLogPageSmartHealthInfoCommand()
        : StorageNVMeGetLogPageCommandBase(NVMeGetLogPageID::SMARTHealthInfo) { }

    ~StorageNVMeGetLogPageSmartHealthInfoCommand() override = default;

    NVMeSmartData                           GetParsedData();
};

}



#endif //OCP_DIAG_SSD_OSV_STORAGE_NVME_COMMAND_GET_LOG_PAGE_SMART_HEALTH_INFO_H
