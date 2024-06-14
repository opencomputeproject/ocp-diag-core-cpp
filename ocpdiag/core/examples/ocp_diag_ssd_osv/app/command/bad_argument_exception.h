// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_BAD_ARGUMENT_EXCEPTION_H
#define OCP_DIAG_SSD_OSV_BAD_ARGUMENT_EXCEPTION_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/ocp_diag_ssd_osv_exception.h"

namespace ocp_diag_ssd_osv {

class BadArgumentException : public OcpDiagSsdException {
public:
    explicit BadArgumentException(const std::string& cause) : OcpDiagSsdException(cause) { }
    ~BadArgumentException() noexcept override = default;
};

}

#endif //ocp_diag_ssd_osv_BAD_ARGUMENT_EXCEPTION_H
