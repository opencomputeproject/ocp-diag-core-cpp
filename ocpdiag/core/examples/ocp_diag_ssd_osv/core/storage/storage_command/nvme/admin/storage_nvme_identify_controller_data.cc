// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_identify_controller_data.h"

namespace ocp_diag_ssd_osv {

NVMeIdentifyControllerData NVMeIdentifyControllerData::From(std::vector<unsigned char> &idBuffer) {

    NVMeIdentifyControllerData result{};

    result.vendorId = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 0, 2));

    result.serialNumber = BufferUtils::GetStringFromDataBuffer(idBuffer, SN_OFFSET, SN_LENGTH, true);
    result.modelNumber = BufferUtils::GetStringFromDataBuffer(idBuffer, MODEL_OFFSET, MODEL_LENGTH, false);
    result.modelNumber.erase(result.modelNumber.find_last_not_of(" ") + 1);
    result.fwRev = BufferUtils::GetStringFromDataBuffer(idBuffer, FW_OFFSET, FW_LENGTH, false);

    result.ssvid = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 2, 2));

    result.rab = idBuffer[8];

    result.ieee = static_cast<unsigned int>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 73, 3));

    result.cmic = idBuffer[76];

    result.mdts = idBuffer[77];

    result.cntlid = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 78, 2));

    result.ver = static_cast<unsigned int>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 80, 4));

    result.rtd3r = static_cast<unsigned int>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 84, 4));

    result.rtd3e = static_cast<unsigned int>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 88, 4));

    result.oaes = static_cast<unsigned int>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 92, 4));

    result.ctratt = static_cast<unsigned int>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 96, 4));

    result.fguid = BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 112, 6);

    result.oacs = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 256, 2));

    result.acl = idBuffer[258];

    result.aerl = idBuffer[259];

    result.frmw = idBuffer[260];

    result.lpa = idBuffer[261];

    result.elpe = idBuffer[262];

    result.npss = idBuffer[263];

    result.avscc = idBuffer[264];

    result.apsta = idBuffer[265];

    result.wctemp = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 266, 2));

    result.cctemp = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 268, 2));

    result.mtfa = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 270, 2));

    result.hmpre = static_cast<unsigned int>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 272, 4));

    result.hmmin = static_cast<unsigned int>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 276, 4));

    result.tnvmcap = BufferUtils::GetBigIntFromByteBufferLittleEndian(idBuffer, 280, 16);

    result.unvmcap = BufferUtils::GetBigIntFromByteBufferLittleEndian(idBuffer, 296, 16);

    result.rpmbs = static_cast<unsigned int>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 312, 4));

    result.edstt = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 316, 2));

    result.dsto = idBuffer[318];

    result.fwug = idBuffer[319];

    result.kas = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 320, 2));

    result.hctma = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 322, 2));

    result.mntmt = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 324, 2));

    result.mxtmt = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 326, 2));

    result.sanicap = static_cast<unsigned int>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 328, 4));

    result.sqes = idBuffer[512];

    result.cqes = idBuffer[513];

    result.maxcmd = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 514, 2));

    result.nn = static_cast<unsigned int>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 516, 4));

    // Optional NVM Command Support (ONCS)
    result.oncs = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 520, 2));

    result.fuses = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 522, 2));

    // Format NVM Attributes (524)
    result.fna = idBuffer[524];

    result.vwc = idBuffer[525];

    result.awun = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 526, 2));

    result.awupf = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 528, 2));

    result.nvscc = idBuffer[530];

    result.acwu = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 532, 2));

    result.sgls = static_cast<unsigned int>(BufferUtils::GetNumberFromByteBufferLittleEndian(idBuffer, 536, 4));

    // OptionRom
    {
        result.optionRomVersion = BufferUtils::GetStringFromDataBuffer(idBuffer, 3080, 8);
    }

    return result;
}

}


