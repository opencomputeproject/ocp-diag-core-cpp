// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_DIAGNOSTIC_MANAGER_H
#define OCP_DIAG_SSD_OSV_DIAGNOSTIC_MANAGER_H

#include <vector>
#include <memory>
#include "ocpdiag/core/results/test_run.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/storage_diagnostic_provider_interface.h"
#include "ocpdiag/core/results/test_step.h"

namespace ocp_diag_ssd_osv {

class DiagnosticManager {
public:
    DiagnosticManager(ocpdiag::results::TestRun& testRunRef,
                      const std::vector<std::shared_ptr<StorageDiagnosticProviderInterface>>& diagnosisProviders /* Test Scenario */) :
                      diagnosisProviders_(diagnosisProviders), testRunRef_(testRunRef) { }

public:
    void                                                                DoDiagnose(std::vector<std::shared_ptr<StorageInfoBase>>& targetStorageInfos);
    std::vector<std::shared_ptr<StorageDiagnosticProviderInterface>>    GetDiagnosisProviders() { return diagnosisProviders_; }

private:
    // Convert DiagnosticLogBase to corresponding ocpdiag::results::xxx
    static void                                                         AddDiagnosticLogToOcpDiagTestStep(ocpdiag::results::TestStep& stepRef,
                                                                                                  std::vector<std::vector<std::shared_ptr<DiagnosticLogBase>>>& diagnosticResults);

private:
    std::vector<std::shared_ptr<StorageDiagnosticProviderInterface>>    diagnosisProviders_;
    ocpdiag::results::TestRun&                                          testRunRef_;
};

}


#endif //OCP_DIAG_SSD_OSV_DIAGNOSTIC_MANAGER_H
