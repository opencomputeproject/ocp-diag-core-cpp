// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "diagnostic_error_log.h"

namespace ocp_diag_ssd_osv {

ocpdiag::results::Error DiagnosticErrorLog::ToOcpDiagError() const {
    return ocpdiag::results::Error{
            .symptom = GetSymptom(),
            .message = GetMessage()
    };
}

}


