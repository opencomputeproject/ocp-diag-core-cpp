// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_STORAGE_NVME_IDENTIFY_CONTROLLER_DATA_H
#define OCP_DIAG_SSD_OSV_STORAGE_NVME_IDENTIFY_CONTROLLER_DATA_H

#include <string>
#include <vector>
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/utils/BigInt.hpp"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/utils/buffer_utils.h"

namespace ocp_diag_ssd_osv {

struct NVMeIdentifyControllerData
{
    enum : size_t
    {
        SN_OFFSET = 4,
        SN_LENGTH = 20,
        MODEL_OFFSET = 24,
        MODEL_LENGTH = 40,
        FW_OFFSET = 64,
        FW_LENGTH = 8,

        CONT_ID_OFFSET = 16,
        CONT_ID_LENGTH = 16,
    };

    std::string						        serialNumber;
    std::string						        modelNumber;
    std::string						        fwRev;

    // Bytes 01:00  PCI Vendor ID (VID)
    unsigned short							vendorId = 0;

    // Bytes 03:02	PCI Subsystem Vendor ID (SSVID)
    unsigned short							ssvid = 0;

    // Bytes 72 Recommended Arbitration Burst(RAB)
    unsigned char							rab = 0;

    // Bytes 75:73	IEEE OUI Identifier(IEEE)
    unsigned int							ieee = 0;

    // Bytes 76 Controller Multi-Path I/O and Namespace Sharing Capabilities (CMIC)
    unsigned char							cmic = 0;

    // Bytes 77 Maximum Data Transfer Size (MDTS)
    unsigned char							mdts = 0;

    // Bytes 79:78 Controller ID(CNTLID)
    unsigned short							cntlid = 0;

    // Bytes 83:80 Version(VER)
    unsigned int							ver = 0;
    unsigned short							GetMajorVersion() const                             { return static_cast<unsigned short>((ver & 0xFFFF0000) >> 16); }
    unsigned char							GetMinorVersion() const                             { return static_cast<unsigned char>((ver & 0xFF00) >> 8); }
    unsigned char							GetTeriaryVersion() const                           { return static_cast<unsigned char>(ver & 0xFF); }

    // Bytes 87:84 RTD3 Resume Latency(RDT3R)
    unsigned int							rtd3r = 0;

    // Bytes 91:88 RTD3 Entry Latency(RTD3E)
    unsigned int							rtd3e = 0;

    // Bytes 95:92 Optional Asynchronous Events Supported(OAES)
    unsigned int							oaes = 0;

    // Bytes 99:96 Controller Attributes (CTRATT)
    unsigned int							ctratt = 0;

    // Bytes 127:112 FRU Globally Unique Identifier (FGUID)
    unsigned long long						fguid = 0;


    // Bytes 257:256 Optional Admin Command Support(OACS)
    unsigned short							oacs = 0;;
    bool							        IsSupportDoorbellBufferConfig() const		        { return oacs & (1 << 8); }
    bool							        IsSupportVirtualizationManagement() const	        { return oacs & (1 << 7); }
    bool							        IsSupportNVMeMISendReceive() const			        { return oacs & (1 << 6); }
    bool							        IsSupportDirectives() const					        { return oacs & (1 << 5); }
    bool							        IsSupportDeviceSelfTest() const				        { return oacs & (1 << 4); }
    bool							        IsSupportNamespaceManagement() const		        { return oacs & (1 << 3); }
    bool							        IsSupportFirmwareCommitAndDownload() const	        { return oacs & (1 << 2); }
    bool							        IsSupportFormatNVM() const					        { return oacs & (1 << 1); }
    bool							        IsSupportSecuritySendReceive() const		        { return oacs & (1 << 0); }

    // Bytes 258 Abort Command Limit(ACL)
    unsigned char							acl = 0;

    // Bytes 259 Asynchronous Event Request Limit(AERL)
    unsigned char							aerl = 0;

    // Bytes 260 Firmware Updates (FRMW)
    unsigned char							frmw = 0;
    bool							        IsSupportFirmwareActivationWithoutReset() const	    { return frmw & (1 << 4); }	// Bit 4
    int								        GetNumberOfSupportFWSlots() const					{ return (frmw >> 1) & (0x7); }	// Bit 3:1
    bool							        IsFirstFWSlotReadOnly() const						{ return frmw & (1 << 0); } // Bit 0

    // Bytes 261 Log Page Attributes (LPA)
    unsigned char							lpa = 0;
    bool							        IsSupportTelemetry()						        { return lpa & (1 << 3); }

    // Bytes 262 Error Log Page Entries (ELPE)
    unsigned char							elpe = 0;

    // Bytes 263 Number Of Power States Support (NPSS)
    unsigned char							npss = 0;

    // Bytes 264 Admin Vendor Specific Command Configuration (AVSCC)
    unsigned char							avscc = 0;

    // Bytes 265 Autonomous Power State Transition Attributes (APSTA)
    unsigned char							apsta = 0;

    // Bytes 267:266 Warning Composite Temperature Threshold (WCTEMP)
    unsigned short							wctemp = 0;

    // Bytes 269:268 Critical Composite Temperature Threshold (CCTEMP)
    unsigned short							cctemp = 0;

    // Bytes 271:270 Maximum Time for Firmware Activation (MTFA)
    unsigned short							mtfa = 0;

    // Bytes 275:272 Host Memory Buffer Preferred Size (HMPRE)
    unsigned int							hmpre = 0;

    // Bytes 279:276 Host Memory Buffer Minimum Size (HMMIN)
    unsigned int							hmmin = 0;

    // Bytes 295:280 Total NVM Capacity (TNVMCAP) - Byte
    BigInt							        tnvmcap = {};

    // Bytes 311:296 Unallocated NVM Capacity (UNVMCAP)
    BigInt							        unvmcap = {};

    // Bytes 315:312 Replay Protected Memory Block Support (RPMBS)
    unsigned int							rpmbs = 0;

    // Bytes 317:316 Extended Device Self-test Time (EDSTT)
    unsigned short							edstt = 0;

    // Bytes 318 Device Self-test Options (DSTO)
    unsigned char							dsto = 0;

    // Bytes 319 Firmware Update Granularity (FWUG)
    unsigned char							fwug = 0;

    // Bytes 321:320 Keep Alive Support (KAS)
    unsigned short							kas = 0;

    // Bytes 323:322 Host Controlled Thermal Management Attributes (HCTMA)
    unsigned short							hctma = 0;

    // Bytes 325:324 Minimum Thermal Management Temperature (MNTMT)
    unsigned short							mntmt = 0;

    // Bytes 327:326 Maximum Thermal Management Temperature (MXTMT)
    unsigned short							mxtmt = 0;

    // Bytes 331:328 Sanitize Capabilities (SANICAP)
    unsigned int							sanicap = 0;

    // Bytes 512 Submission Queue Entry Size (SQES)
    unsigned char							sqes = 0;

    // Bytes 513 Completion Queue Entry Size (CQES)
    unsigned char							cqes = 0;

    // Bytes 515:514 Maximum Outstanding Commands (MAXCMD)
    unsigned short							maxcmd = 0;

    // Bytes 519:516 Number of Namespaces (NN)
    unsigned int							nn = 0;

    // Bytes 521:520 Optional NVM Command Support (ONCS)
    unsigned short							oncs = 0;
    bool							        IsSupportTimeStampFeature() const				{ return oncs & (1 << 6); } // Bit 6
    bool							        IsSupportReservation() const					{ return oncs & (1 << 5); } // Bit 5
    bool							        IsSupportNonZeroValueInFeaturesCommand() const	{ return oncs & (1 << 4); }	// Bit 4
    bool							        IsSupportWriteZero() const						{ return oncs & (1 << 3); }	// Bit 3
    bool							        IsSupportDatasetManagement() const				{ return oncs & (1 << 2); }	// Bit 2
    bool							        IsSupportWriteUncorrectable() const				{ return oncs & (1 << 1); }	// Bit 1
    bool							        IsSupportCompare() const						{ return oncs & (1 << 0); }	// Bit 0

    // Bytes 523:522 Fused Operation Support (FUSES)
    unsigned short							fuses = 0;

    // Bytes 524 Format NVM Attributes (FNA)
    unsigned char							fna = 0;
    bool							        IsCryptoGraphicEraseSupport() const				{ return (fna & (1 << 2)); }// Bit 2
    bool							        IsSecureEraseAppliesToAllNameSpace() const		{ return (fna & (1 << 1)); }// Bit 1
    bool							        IsFormatAppliesToAllNameSpace() const			{ return (fna & (1 << 0)); }// Bit 0

    // Bytes 525 Volatile Write Cache(VWC)
    unsigned char							vwc = 0;

    // Bytes 527:526 Atomic Write Unit Normal (AWUN)
    unsigned short							awun = 0;

    // Bytes 529:528 Atomic Write Unit Power Fail (AWUPF)
    unsigned short							awupf = 0;

    // Bytes 530 NVM Vendor Specific Command Configuration (NVSCC) 
    unsigned char							nvscc = 0;

    // Bytes 533:532 Atomic Compare & Write Unit (ACWU)
    unsigned short							acwu = 0;

    // Bytes 539:536 SGL Support (SGLS)
    unsigned int							sgls = 0;

    // 3087:3080
    std::string							    optionRomVersion;

    static NVMeIdentifyControllerData       From(std::vector<unsigned char>& idBuffer);
};

}

#endif // OCP_DIAG_SSD_OSV_STORAGE_NVME_IDENTIFY_CONTROLLER_DATA_H
