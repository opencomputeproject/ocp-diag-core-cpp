// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_command_identify_namespace.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/utils/buffer_utils.h"
#include <algorithm>

namespace ocp_diag_ssd_osv {

StorageNVMeIdentifyNamespaceCommand::StorageNVMeIdentifyNamespaceCommand(unsigned int namespaceId)
        : StorageNVMeIdentifyCommandBase(StorageNVMeIdentifyCnsType::Namespace) {
    SetNamespaceId(namespaceId);
}

std::string StorageNVMeIdentifyNamespaceCommand::GetCommandName() {
    return "NVMe Identify Namespace #" + std::to_string(static_cast<long long>(GetNamespaceId()));
}

NVMeIdentifyNamespaceData StorageNVMeIdentifyNamespaceCommand::GetParsedData() {

    NVMeIdentifyNamespaceData result{};

    result.namespaceId = GetNamespaceId();

    // If all data is zero, this namespace is not active namespace. (by NVMe spec)
    result.isActiveNameSpace = !(std::all_of(dataBuffer_.begin(), dataBuffer_.end(), [](int i) { return i == 0; }));

    result.nsze = BufferUtils::GetNumberFromByteBufferLittleEndian(dataBuffer_, 0, 8);

    result.ncap = BufferUtils::GetNumberFromByteBufferLittleEndian(dataBuffer_, 8, 8);

    result.nuse = BufferUtils::GetNumberFromByteBufferLittleEndian(dataBuffer_, 16, 8);

    result.nsfeat = dataBuffer_[24];

    result.nlbaf = dataBuffer_[25];

    result.flbas = dataBuffer_[26];

    result.mc = dataBuffer_[27];

    result.dpc = dataBuffer_[28];

    result.dps = dataBuffer_[29];

    result.nmic = dataBuffer_[30];

    result.rescap = dataBuffer_[31];

    result.fpi = dataBuffer_[32];

    result.dlfeat = dataBuffer_[33];

    result.nawun = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(dataBuffer_, 34, 2));

    result.nawupf = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(dataBuffer_, 36, 2));

    result.nacwu = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(dataBuffer_, 38, 2));

    result.nabsn = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(dataBuffer_, 40, 2));

    result.nabo = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(dataBuffer_, 42, 2));

    result.nabspf = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(dataBuffer_, 44, 2));

    result.noiob = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(dataBuffer_, 46, 2));

    result.nvmcap = BufferUtils::GetBigIntFromByteBufferLittleEndian(dataBuffer_, 48, 16);

    result.nguid = BufferUtils::GetBigIntFromByteBufferLittleEndian(dataBuffer_, 104, 16);

    result.eui64 = BufferUtils::GetNumberFromByteBufferLittleEndian(dataBuffer_, 120, 8);

    result.lbaFormatNSupport.clear();

    for (unsigned int i = 0; i <= result.nlbaf; i++)
    {
        const unsigned int startBytes = 128 + (i * 4);

        NVMeLBAFormatDataStructure lbaFormatData {
                static_cast<unsigned char>(dataBuffer_[startBytes + 3] & 0x3u),
                dataBuffer_[startBytes + 2],
                static_cast<unsigned short>((dataBuffer_[startBytes + 1] << 8) | (dataBuffer_[startBytes]))
        };

        result.lbaFormatNSupport.push_back(lbaFormatData);
    }

    return result;

}

}

