// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_identify_namespace_data.h"

namespace ocp_diag_ssd_osv {

unsigned char NVMeIdentifyNamespaceData::GetCurrentLbaFormatIndex() const {
    return flbas & 0xF;
}

NVMeLBAFormatDataStructure NVMeIdentifyNamespaceData::GetCurrentLbaFormat() const{
    return lbaFormatNSupport[GetCurrentLbaFormatIndex()];
}

bool NVMeIdentifyNamespaceData::IsMetadataAtEndOfLBA() const {
    return (flbas & 0x10) >> 4; // bit4
}

bool NVMeIdentifyNamespaceData::IsProtectionInformationEnabled() const {
    // 000b = Protection Information is not enabled
    // 001b = Protection Information is enabled, Type 1
    // 010b = Protection Information is enabled, Type 2
    // 011b = Protection Information is enabled, Type 3
    return (dps & 0x7) != 0x00;
}

}


