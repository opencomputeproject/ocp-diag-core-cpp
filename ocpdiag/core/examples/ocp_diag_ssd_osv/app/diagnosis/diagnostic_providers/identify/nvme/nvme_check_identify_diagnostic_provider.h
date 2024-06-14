// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_NVME_CHECK_IDENTIFY_DIAGNOSTIC_PROVIDER_H
#define OCP_DIAG_SSD_OSV_NVME_CHECK_IDENTIFY_DIAGNOSTIC_PROVIDER_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_identify_controller_data.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/storage_diagnostic_provider_interface.h"

namespace ocp_diag_ssd_osv {

// Class for diagnose SSD using NVMe Identify Controller Data
class NVMeCheckIdentifyDiagnosticProvider : public StorageDiagnosticProviderInterface {

public:
    NVMeCheckIdentifyDiagnosticProvider() = default;
    ~NVMeCheckIdentifyDiagnosticProvider() override = default;

    std::vector<std::shared_ptr<DiagnosticLogBase>>     Diagnose(std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo) override;
    std::string                                         GetDiagnosisName() override { return "nvme-identify-controller-check"; }
    bool                                                Supports(std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo) override;

protected:
    bool                                                ExecuteIdentifyControllerCommand(std::vector<std::shared_ptr<DiagnosticLogBase>>& result,
                                                                                         NVMeIdentifyControllerData& identifyControllerDataResult,
                                                                                         std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo);
};

}


#endif //OCP_DIAG_SSD_OSV_NVME_CHECK_IDENTIFY_DIAGNOSTIC_PROVIDER_H
