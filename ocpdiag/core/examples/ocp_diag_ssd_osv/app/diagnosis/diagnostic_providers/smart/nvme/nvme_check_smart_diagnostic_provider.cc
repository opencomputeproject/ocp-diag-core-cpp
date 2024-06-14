// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/smart/nvme/nvme_check_smart_diagnostic_provider.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_command_get_log_page_smart_health_info.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_result_log.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_measurement_log.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_error_log.h"
#include <cassert>

namespace ocp_diag_ssd_osv {
std::vector<std::shared_ptr<DiagnosticLogBase>> NVMeCheckSmartDiagnosticProvider::Diagnose(std::shared_ptr<StorageInfoBase> storageInfo) {

    std::vector<std::shared_ptr<DiagnosticLogBase>> result;
    if(testType_ == TestType::Unknown) {
        assert(false && "Test Type is Unknown");
    }
    else
    {
        NVMeSmartData nvmeSmartData{};
        if(ExecuteGetLogPageCommand(result, nvmeSmartData, storageInfo))
        {
            CheckCapFail(result, nvmeSmartData, storageInfo);
            CheckReadOnlyMode(result, nvmeSmartData,  storageInfo);
            CheckAvailableSpare(result, nvmeSmartData,  storageInfo);
            CheckPercentageUsed(result, nvmeSmartData,  storageInfo);
        }
    }

    return result;

}

bool NVMeCheckSmartDiagnosticProvider::Supports(std::shared_ptr<StorageInfoBase> storageInfo) {
    return storageInfo->GetStorageProtocol() == StorageProtocol::NVMe;
}

bool NVMeCheckSmartDiagnosticProvider::ExecuteGetLogPageCommand(std::vector<std::shared_ptr<DiagnosticLogBase>>& result,
                                                                NVMeSmartData& nvmeSmartDataResult,
                                                                std::shared_ptr<StorageInfoBase> storageInfo) {

    bool ret = true;

    RAIIHandle handle = storageInfo->CreateDeviceHandle();
    std::shared_ptr<StorageNVMeGetLogPageSmartHealthInfoCommand> smartCommand =
            std::make_shared<StorageNVMeGetLogPageSmartHealthInfoCommand>();

    // NVMe Get Log Page Command must be successful
    if(storageInfo->SendStorageCommand(handle, smartCommand)) {
        result.push_back(std::make_shared<DiagnosticResultLog>(storageInfo,
                                                               DiagnosisResultType::Pass,
                                                               "nvme-get-log-page-smart-health-info-command-pass",
                                                               "NVMe Get Log Page command has been sent successfully."));
        nvmeSmartDataResult = smartCommand->GetParsedData();
    }
    else {
        result.push_back(std::make_shared<DiagnosticResultLog>(storageInfo,
                                                               DiagnosisResultType::Fail,
                                                               "nvme-get-log-page-smart-health-info-command-fail",
                                                               smartCommand->GetStorageCommandResult()->GetCommandNameAndErrorCodeString()));
        ret = false;
    }

    return ret;
}

void ocp_diag_ssd_osv::NVMeCheckSmartDiagnosticProvider::CheckCapFail(std::vector<std::shared_ptr<DiagnosticLogBase>>& result,
                                                                  const NVMeSmartData& nvmeSmartDataResult,
                                                                  std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo) {


    bool isCapFail = (nvmeSmartDataResult.criticalWarning & 0x10) >> 4;
    result.push_back(std::make_shared<DiagnosticMeasurementLog>(storageInfo,
                                                                "check-capacitor-status",
                                                                "Bit",
                                                                std::vector<StorageTestMeasurementValidator> {
                                                                        StorageTestMeasurementValidator{
                                                                                StorageTestValidatorType::NotEqual,
                                                                                { false },
                                                                                "nvme_smart_critical_warning_bit_4"
                                                                        }
                                                                },
                                                                isCapFail));

}

void ocp_diag_ssd_osv::NVMeCheckSmartDiagnosticProvider::CheckReadOnlyMode(std::vector<std::shared_ptr<DiagnosticLogBase>>& result,
                                                                       const NVMeSmartData& nvmeSmartDataResult,
                                                                       std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo) {

    bool isReadOnlyMode = (nvmeSmartDataResult.criticalWarning & 0x8) >> 3;
    result.push_back(std::make_shared<DiagnosticMeasurementLog>(storageInfo,
                                                                "check-read-only-mode",
                                                                "Bit",
                                                                std::vector<StorageTestMeasurementValidator> {
                                                                        StorageTestMeasurementValidator{
                                                                                StorageTestValidatorType::NotEqual,
                                                                                { false },
                                                                                "nvme_smart_critical_warning_bit_3"
                                                                        }
                                                                },
                                                                isReadOnlyMode));

}

void ocp_diag_ssd_osv::NVMeCheckSmartDiagnosticProvider::CheckAvailableSpare(std::vector<std::shared_ptr<DiagnosticLogBase>>& result,
                                                                         const NVMeSmartData& nvmeSmartDataResult,
                                                                         std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo) {

    auto availableSpare = static_cast<double>(nvmeSmartDataResult.availableSpare);
    auto availableSpareThreshold = static_cast<double>(nvmeSmartDataResult.availableSpareThreshold);

    switch(testType_)
    {
        case TestType::Initial:
            result.push_back(std::make_shared<DiagnosticMeasurementLog>(storageInfo,
                                                                        "check-available-spare",
                                                                        "Percentage",
                                                                        std::vector<StorageTestMeasurementValidator> {
                                                                                StorageTestMeasurementValidator{
                                                                                        StorageTestValidatorType::Equal,
                                                                                        { 100.0f },
                                                                                        "nvme_smart_available_spare_100_percent"
                                                                                }
                                                                        },
                                                                        availableSpare));
            break;
        case TestType::Field:
            result.push_back(std::make_shared<DiagnosticMeasurementLog>(storageInfo,
                                                                        "check-available-spare",
                                                                        "Percentage",
                                                                        std::vector<StorageTestMeasurementValidator> {
                                                                                StorageTestMeasurementValidator{
                                                                                        StorageTestValidatorType::GreaterThanOrEqual,
                                                                                        { availableSpareThreshold },
                                                                                        "nvme_smart_available_spare_threshold"
                                                                                }
                                                                        },
                                                                        availableSpare));
            break;
        default:
            assert(false && "Invalid TestType");
            break;
    }

}

void ocp_diag_ssd_osv::NVMeCheckSmartDiagnosticProvider::CheckPercentageUsed(std::vector<std::shared_ptr<DiagnosticLogBase>>& result,
                                                                         const NVMeSmartData& nvmeSmartDataResult,
                                                                         std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo) {
    result.push_back(std::make_shared<DiagnosticMeasurementLog>(storageInfo,
                                                                "check-percentage-used",
                                                                "Percentage",
                                                                std::vector<StorageTestMeasurementValidator> {
                                                                        StorageTestMeasurementValidator{
                                                                                StorageTestValidatorType::LessThan,
                                                                                { 100.0f },
                                                                                "nvme_smart_percentage_used_100_percent"
                                                                        }
                                                                },
                                                                static_cast<double>(nvmeSmartDataResult.percentageUsed)));
}

}


