// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "diagnostic_log_base.h"

namespace ocp_diag_ssd_osv {

DiagnosticLogBase::DiagnosticLogBase(StorageTestLogType storageTestLogType,
                                     const std::shared_ptr<StorageInfoBase> &storageInfo) :
    storageTestLogType_(storageTestLogType), registeredHardwareInfo_(storageInfo->GetRegisteredHardwareInfoRef()) {

}

}

