// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/identify/nvme/nvme_check_identify_diagnostic_provider.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_command_identify_controller.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_result_log.h"

namespace ocp_diag_ssd_osv {

std::vector<std::shared_ptr<DiagnosticLogBase>> NVMeCheckIdentifyDiagnosticProvider::Diagnose(std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo) {

    std::vector<std::shared_ptr<DiagnosticLogBase>> result;
    NVMeIdentifyControllerData identifyControllerData{};

    if(!ExecuteIdentifyControllerCommand(result, identifyControllerData, storageInfo))
        return result;

    // If Identity Controller Command is successful, a test using this data can be performed.

    return result;
}

bool NVMeCheckIdentifyDiagnosticProvider::Supports(std::shared_ptr<StorageInfoBase> storageInfo) {
    return storageInfo->GetStorageProtocol() == StorageProtocol::NVMe;
}

bool NVMeCheckIdentifyDiagnosticProvider::ExecuteIdentifyControllerCommand(std::vector<std::shared_ptr<DiagnosticLogBase>>& result,
                                                                           NVMeIdentifyControllerData& identifyControllerDataResult,
                                                                           std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo) {
    bool ret = true;
    RAIIHandle handle = storageInfo->CreateDeviceHandle();

    // NVMe Identify Controller Command must be successful
    std::shared_ptr<StorageNVMeIdentifyControllerCommand> nvmeIdentifyControllerCmd = std::make_shared<StorageNVMeIdentifyControllerCommand>();

    if(storageInfo->SendStorageCommand(handle, nvmeIdentifyControllerCmd)) {
        result.push_back(std::make_shared<DiagnosticResultLog>(storageInfo,
                                                               DiagnosisResultType::Pass,
                                                               "nvme-identify-controller-command-pass",
                                                               "NVMe Identify Controller command has been sent successfully."));
        identifyControllerDataResult = nvmeIdentifyControllerCmd->GetParsedData(); // Update NVMeIdentifyControllerData
    }
    else {
        result.push_back(std::make_shared<DiagnosticResultLog>(storageInfo,
                                                               DiagnosisResultType::Fail,
                                                               "nvme-identify-controller-command-fail",
                                                               nvmeIdentifyControllerCmd->GetStorageCommandResult()->GetCommandNameAndErrorCodeString()));
        ret = false;
    }

    return ret;
}

}

