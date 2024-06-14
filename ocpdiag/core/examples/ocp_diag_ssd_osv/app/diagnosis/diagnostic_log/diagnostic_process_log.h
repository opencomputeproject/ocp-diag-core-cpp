// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_DIAGNOSTIC_PROCESS_LOG_H
#define OCP_DIAG_SSD_OSV_DIAGNOSTIC_PROCESS_LOG_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_log_base.h"

namespace ocp_diag_ssd_osv {

enum class ProcessLogSeverity
{
    Info,
    Warning,
    Error,
    Fatal,
    Debug,
};

class DiagnosticProcessLog : public DiagnosticLogBase {
public:
    DiagnosticProcessLog(const std::shared_ptr<StorageInfoBase> &storageInfo,
                         ProcessLogSeverity processLogSeverity,
                         const std::string& message)
            : DiagnosticLogBase(StorageTestLogType::Log, storageInfo),
              processLogSeverity_(processLogSeverity), message_(message) {}

    ~DiagnosticProcessLog() override = default;

public:
    ProcessLogSeverity              GetProcessLogSeverity() const { return processLogSeverity_; }
    const std::string&              GetMessage() const { return message_; }

    ocpdiag::results::Log           ToOcpDiagLog() const;

protected:
    ProcessLogSeverity              processLogSeverity_;
    std::string                     message_;
};

}



#endif //FULL_SPEC_DIAGNOSTIC_PROCESS_LOG_H
