// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_DIAGNOSTIC_ERROR_LOG_H
#define OCP_DIAG_SSD_OSV_DIAGNOSTIC_ERROR_LOG_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_log_base.h"

namespace ocp_diag_ssd_osv {

class DiagnosticErrorLog : public DiagnosticLogBase {
public:
    DiagnosticErrorLog(const std::shared_ptr<StorageInfoBase> &storageInfo,
                       const std::string& symptom,
                       const std::string& message)
            : DiagnosticLogBase(StorageTestLogType::Error, storageInfo),
            symptom_(symptom), message_(message) {}

    ~DiagnosticErrorLog() override = default;

public:
    const std::string&          GetSymptom() const { return symptom_; }
    const std::string&          GetMessage() const { return message_; }

    ocpdiag::results::Error     ToOcpDiagError() const;

protected:
    std::string                 symptom_;
    std::string                 message_;
};

}

#endif //OCP_DIAG_SSD_OSV_DIAGNOSTIC_ERROR_LOG_H
