// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_DIAGNOSTIC_LOG_BASE_H
#define OCP_DIAG_SSD_OSV_DIAGNOSTIC_LOG_BASE_H

#include "ocpdiag/core/results/data_model/input_model.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_model/storage_info_base.h"

namespace ocp_diag_ssd_osv {

enum class StorageTestLogType
{
    Unknown,
    Diagnosis,
    Measurement,
    Error,
    Log,
};

class DiagnosticLogBase {
public:
    explicit DiagnosticLogBase(StorageTestLogType storageTestLogType, const std::shared_ptr<StorageInfoBase>& storageInfo);
    virtual ~DiagnosticLogBase() = default;

public:
    StorageTestLogType                                  GetStorageTestLogType() const { return storageTestLogType_; }
    const ocpdiag::results::RegisteredHardwareInfo&     GetRegisteredHardwareInfoRef() const { return registeredHardwareInfo_; }

protected:
    StorageTestLogType                                  storageTestLogType_;
    ocpdiag::results::RegisteredHardwareInfo            registeredHardwareInfo_;
};

}

#endif // OCP_DIAG_SSD_OSV_DIAGNOSTIC_LOG_BASE_H
