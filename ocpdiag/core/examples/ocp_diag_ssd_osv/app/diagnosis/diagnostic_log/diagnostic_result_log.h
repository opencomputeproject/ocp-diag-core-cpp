// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_DIAGNOSTIC_RESULT_LOG_H
#define OCP_DIAG_SSD_OSV_DIAGNOSTIC_RESULT_LOG_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_log_base.h"

namespace ocp_diag_ssd_osv {

enum class DiagnosisResultType
{
    Unknown,
    Pass,
    Fail
};

class DiagnosticResultLog : public DiagnosticLogBase {
public:
    DiagnosticResultLog(const std::shared_ptr<StorageInfoBase> &storageInfo,
                        DiagnosisResultType diagnosisResultType,
                        const std::string& verdict,
                        const std::string& message);

    ~DiagnosticResultLog() override = default;

public:
    DiagnosisResultType             GetDiagnosisResultType() const { return diagnosisResultType_; }
    const std::string&              GetVerdict() const { return verdict_; }
    const std::string&              GetMessage() const { return message_; }

    ocpdiag::results::Diagnosis     ToOcpDiagDiagnosis() const;

protected:
    DiagnosisResultType             diagnosisResultType_;
    std::string                     verdict_;
    std::string                     message_;
};

}

#endif //OCP_DIAG_SSD_OSV_DIAGNOSTIC_RESULT_LOG_H
