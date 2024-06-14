// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_STORAGE_DIAGNOSTIC_PROVIDER_INTERFACE_H
#define OCP_DIAG_SSD_OSV_STORAGE_DIAGNOSTIC_PROVIDER_INTERFACE_H

#include <string>
#include <memory>
#include <vector>
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_model/storage_info_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_log_base.h"

namespace ocp_diag_ssd_osv {

// Indicates a class can process a specific Storage Diagnostic implementation.
class StorageDiagnosticProviderInterface {
public:
    StorageDiagnosticProviderInterface() = default;
    virtual ~StorageDiagnosticProviderInterface() = default;

public:
    virtual std::vector<std::shared_ptr<DiagnosticLogBase>>     Diagnose(std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfos) = 0;
    virtual std::string                                         GetDiagnosisName() = 0;

    // Returns true if this StorageDiagnosticProvider supports the indicated StorageInfo object.
    virtual bool                                                Supports(std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo) = 0;
};

}


#endif //OCP_DIAG_SSD_OSV_STORAGE_DIAGNOSTIC_PROVIDER_INTERFACE_H
