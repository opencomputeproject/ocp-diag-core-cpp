// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "diagnostic_result_log.h"
#include <cassert>

namespace ocp_diag_ssd_osv {

DiagnosticResultLog::DiagnosticResultLog(const std::shared_ptr<StorageInfoBase> &storageInfo,
                                         DiagnosisResultType diagnosisResultType,
                                         const std::string& verdict, const std::string& message)
        : DiagnosticLogBase(StorageTestLogType::Diagnosis, storageInfo),
          diagnosisResultType_(diagnosisResultType), verdict_(verdict), message_(message) {
}

ocpdiag::results::Diagnosis DiagnosticResultLog::ToOcpDiagDiagnosis() const {

    ocpdiag::results::DiagnosisType diagnosisType;

    switch(GetDiagnosisResultType())
    {
        case DiagnosisResultType::Unknown:
            diagnosisType = ocpdiag::results::DiagnosisType::kUnknown;
            break;
        case DiagnosisResultType::Pass:
            diagnosisType = ocpdiag::results::DiagnosisType::kPass;
            break;
        case DiagnosisResultType::Fail:
            diagnosisType = ocpdiag::results::DiagnosisType::kFail;
            break;
        default:
            assert(false && "Invalid DiagnosisResultType");
            break;
    }

    return ocpdiag::results::Diagnosis{
            .verdict = GetVerdict(),
            .type = diagnosisType,
            .message = GetMessage(),
            .hardware_info = GetRegisteredHardwareInfoRef()
    };
}

}
