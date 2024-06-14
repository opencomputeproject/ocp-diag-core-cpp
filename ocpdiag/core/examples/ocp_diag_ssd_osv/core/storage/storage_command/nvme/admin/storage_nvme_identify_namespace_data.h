// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_STORAGE_NVME_IDENTIFY_NAMESPACE_DATA_H
#define OCP_DIAG_SSD_OSV_STORAGE_NVME_IDENTIFY_NAMESPACE_DATA_H

#include <cmath>
#include <vector>
#include <string>
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/utils/BigInt.hpp"

namespace ocp_diag_ssd_osv {

struct NVMeLBAFormatDataStructure
{
    unsigned char	relativePerformance = 0;	// Bits 25:24 - Relative Performance (RP)
    unsigned char	lbaDataSizePowerOfTwo = 0;	// Bits 23:16 - LBA Data Size (LBADS). The value is reported in terms of a power of two(2 ^ n).
    unsigned short	metaDataSize = 0;			// Bits 15:00 - Metadata Size(MS)

    NVMeLBAFormatDataStructure(unsigned char relativePerformance, unsigned char lbaDataSizePowerOfTwo, unsigned short metaDataSize)
            : relativePerformance(relativePerformance),
              lbaDataSizePowerOfTwo(lbaDataSizePowerOfTwo),
              metaDataSize(metaDataSize) {}

    unsigned int GetLbaDataSize()
    {
        return static_cast<unsigned int>(pow(2, lbaDataSizePowerOfTwo));
    }
};

struct NVMeIdentifyNamespaceData {
    // Bytes 7:0 Namespace Size (NSZE)
    unsigned long long						nsze = 0;

    // Bytes 15:8 Namespace Capacity (NCAP)
    unsigned long long						ncap = 0;

    // Bytes 23:16 Namespace Utilization (NUSE)
    unsigned long long						nuse = 0;

    // Bytes 24 Namespace Features (NSFEAT)
    unsigned char							nsfeat = 0;

    // Bytes 25 Number of LBA Formats (NLBAF) - Zero base
    unsigned char							nlbaf = 0;

    // Bytes 26 Formatted LBA Size (FLBAS)
    unsigned char							flbas = 0;

    // Bytes 27 Metadata Capabilities(MC)
    unsigned char							mc = 0;

    // Bytes 28 End-to-end Data Protection Capabilities (DPC)
    unsigned char							dpc = 0;

    // Bytes 29 End-to-end Data Protection Type Settings (DPS)
    unsigned char							dps = 0;

    // Bytes 30 Namespace Multi-path I/O and Namespace Sharing Capabilities (NMIC)
    unsigned char							nmic = 0;

    // Bytes 31 Reservation Capabilities (RESCAP)
    unsigned char							rescap = 0;

    // Bytes 32 Format Progress Indicator (FPI)
    unsigned char							fpi = 0;

    // Bytes 33 Deallocate Logical Block Features (DLFEAT)
    unsigned char							dlfeat = 0;

    // Bytes 35:34 Namespace Atomic Write Unit Normal(NAWUN)
    unsigned short							nawun = 0;

    // Bytes 37:36 Namespace Atomic Write Unit Power Fail (NAWUPF)
    unsigned short							nawupf = 0;

    // Bytes 39:38 Namespace Atomic Compare & Write Unit (NACWU)
    unsigned short							nacwu = 0;

    // Bytes 41:40 Namespace Atomic Boundary Size Normal (NABSN)
    unsigned short							nabsn = 0;

    // Bytes 43:42 Namespace Atomic Boundary Offset (NABO)
    unsigned short							nabo = 0;

    // Bytes 45:44 Namespace Atomic Boundary Size Power Fail (NABSPF)
    unsigned short							nabspf = 0;

    // Bytes 47:46 Namespace Optimal IO Boundary (NOIOB)
    unsigned short							noiob = 0;

    // Bytes 63:48 NVM Capacity (NVMCAP)
    BigInt							        nvmcap = 0;

    // Bytes 119:104 Namespace Globally Unique Identifier (NGUID)
    BigInt							        nguid = 0;

    // Bytes 127:120 IEEE Extended Unique Identifier (EUI64)
    unsigned long long						eui64 = 0;

    // Bytes 131:128 LBA Format 0 Support
    // Bytes 135:132 LBA Format 1 Support
    // ...
    // Bytes 191:188 LBA Format 15 Support
    std::vector<NVMeLBAFormatDataStructure>	lbaFormatNSupport;

    unsigned int                            namespaceId;
    bool								    isActiveNameSpace = false;

public:
    unsigned char						    GetCurrentLbaFormatIndex() const;
    NVMeLBAFormatDataStructure			    GetCurrentLbaFormat() const;
    bool								    IsMetadataAtEndOfLBA() const;
    bool								    IsProtectionInformationEnabled() const;
};

}

#endif //OCP_DIAG_SSD_OSV_STORAGE_NVME_IDENTIFY_NAMESPACE_DATA_H
