// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_manager.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/utils/thread_manager.h"
#include "ocpdiag/core/results/test_step.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_result_log.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_measurement_log.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_error_log.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_process_log.h"
#include <cassert>

namespace ocp_diag_ssd_osv {

void DiagnosticManager::DoDiagnose(
        std::vector<std::shared_ptr<StorageInfoBase>>& targetStorageInfos) {

    // Execute DiagnosisProviders
    for (const auto &diagnosisProvider: this->GetDiagnosisProviders()) {

        // Add Test Step
        ocpdiag::results::TestStep step(diagnosisProvider->GetDiagnosisName(), testRunRef_);

        // For all storages, run diagnosis in each thread.
        std::shared_ptr<ocp_diag_ssd_osv::ThreadManager> threadManager = ocp_diag_ssd_osv::ThreadManager::GetGlobalInstance();
        std::vector<std::vector<std::shared_ptr<DiagnosticLogBase>>> diagnosticResults(targetStorageInfos.size());

        for(size_t targetStorageIndex = 0; targetStorageIndex < targetStorageInfos.size(); targetStorageIndex++) {
            std::shared_ptr<StorageInfoBase>& targetStorageInfo = targetStorageInfos[targetStorageIndex];
            if(diagnosisProvider->Supports(targetStorageInfo)) {
                threadManager->Launch([targetStorageIndex, &targetStorageInfo, &diagnosisProvider, &diagnosticResults]() {
                    diagnosticResults[targetStorageIndex] = diagnosisProvider->Diagnose(targetStorageInfo);
                });
            }
        }
        threadManager->Join();

        // Gather each diagnostic results
        AddDiagnosticLogToOcpDiagTestStep(step, diagnosticResults);
    }

}

void DiagnosticManager::AddDiagnosticLogToOcpDiagTestStep(ocpdiag::results::TestStep& stepRef,
                                                          std::vector<std::vector<std::shared_ptr<DiagnosticLogBase>>>& diagnosticResults) {
    for (const auto& diagnosticResultsForEachStorage: diagnosticResults) {
        for (auto &diagnosticResult: diagnosticResultsForEachStorage) {
            switch(diagnosticResult->GetStorageTestLogType()) {
                case StorageTestLogType::Diagnosis:
                    stepRef.AddDiagnosis(std::dynamic_pointer_cast<DiagnosticResultLog>(
                            diagnosticResult)->ToOcpDiagDiagnosis());
                    break;
                case StorageTestLogType::Measurement:
                    stepRef.AddMeasurement(std::dynamic_pointer_cast<DiagnosticMeasurementLog>(
                            diagnosticResult)->ToOcpDiagMeasurement());
                    break;
                case StorageTestLogType::Error:
                    stepRef.AddError(std::dynamic_pointer_cast<DiagnosticErrorLog>(
                            diagnosticResult)->ToOcpDiagError());
                    break;
                case StorageTestLogType::Log:
                    stepRef.AddLog(std::dynamic_pointer_cast<DiagnosticProcessLog>(
                            diagnosticResult)->ToOcpDiagLog());
                    break;
                case StorageTestLogType::Unknown:
                    assert(false && "Invalid StorageTestLogType");
                    break;
            }

        }
    }
}

}


