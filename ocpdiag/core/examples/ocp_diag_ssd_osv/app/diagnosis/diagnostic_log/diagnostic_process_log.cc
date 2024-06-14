// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "diagnostic_process_log.h"
#include <cassert>

namespace ocp_diag_ssd_osv {

ocpdiag::results::Log DiagnosticProcessLog::ToOcpDiagLog() const {

    ocpdiag::results::LogSeverity logSeverity;

    switch(GetProcessLogSeverity())
    {
        case ProcessLogSeverity::Info:
            logSeverity = ocpdiag::results::LogSeverity::kInfo;
            break;
        case ProcessLogSeverity::Warning:
            logSeverity = ocpdiag::results::LogSeverity::kWarning;
            break;
        case ProcessLogSeverity::Error:
            logSeverity = ocpdiag::results::LogSeverity::kError;
            break;
        case ProcessLogSeverity::Fatal:
            logSeverity = ocpdiag::results::LogSeverity::kFatal;
            break;
        case ProcessLogSeverity::Debug:
            logSeverity = ocpdiag::results::LogSeverity::kDebug;
            break;
        default:
            assert(false && "Invalid ProcessLogSeverity");
            break;
    }

    return ocpdiag::results::Log {
        .severity = logSeverity,
        .message = GetMessage(),
    };
}

}

