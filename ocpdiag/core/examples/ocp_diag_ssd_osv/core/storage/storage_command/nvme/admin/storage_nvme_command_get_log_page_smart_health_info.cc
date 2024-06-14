// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_command_get_log_page_smart_health_info.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/utils/buffer_utils.h"

namespace ocp_diag_ssd_osv {

NVMeSmartData StorageNVMeGetLogPageSmartHealthInfoCommand::GetParsedData() {

    NVMeSmartData result{};

    result.criticalWarning = dataBuffer_[0];

    result.compositeTemperature = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(
            dataBuffer_, 1, 2));

    result.availableSpare = dataBuffer_[3];

    result.availableSpareThreshold = dataBuffer_[4];

    result.percentageUsed = dataBuffer_[5];

    result.dataUnitsRead = BufferUtils::GetBigIntFromByteBufferLittleEndian(dataBuffer_, 32, 16);

    result.dataUnitsWritten = BufferUtils::GetBigIntFromByteBufferLittleEndian(dataBuffer_, 48, 16);

    result.hostReadCommands = BufferUtils::GetBigIntFromByteBufferLittleEndian(dataBuffer_, 64, 16);

    result.hostWriteCommands = BufferUtils::GetBigIntFromByteBufferLittleEndian(dataBuffer_, 80, 16);

    result.controllerBusyTime = BufferUtils::GetBigIntFromByteBufferLittleEndian(dataBuffer_, 96, 16);

    result.powerCycles = BufferUtils::GetBigIntFromByteBufferLittleEndian(dataBuffer_, 112, 16);

    result.powerOnHours = BufferUtils::GetBigIntFromByteBufferLittleEndian(dataBuffer_, 128, 16);

    result.unsafeShutdowns = BufferUtils::GetBigIntFromByteBufferLittleEndian(dataBuffer_, 144, 16);

    result.mediaAndDataIntegrityErrors = BufferUtils::GetBigIntFromByteBufferLittleEndian(dataBuffer_, 160, 16);

    result.numberOfErrorInformationLogEntries = BufferUtils::GetBigIntFromByteBufferLittleEndian(dataBuffer_, 176, 16);

    result.warningCompositeTemperatureTime = static_cast<unsigned int>(BufferUtils::GetNumberFromByteBufferLittleEndian(
            dataBuffer_, 192, 4));

    result.criticalCompositeTemperatureTime = static_cast<unsigned int>(BufferUtils::GetNumberFromByteBufferLittleEndian(
            dataBuffer_, 196, 4));

    result.temperatureSensor1 = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(
            dataBuffer_, 200, 2));

    result.temperatureSensor2 = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(
            dataBuffer_, 202, 2));

    result.temperatureSensor3 = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(
            dataBuffer_, 204, 2));

    result.temperatureSensor4 = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(
            dataBuffer_, 206, 2));

    result.temperatureSensor5 = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(
            dataBuffer_, 208, 2));

    result.temperatureSensor6 = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(
            dataBuffer_, 210, 2));

    result.temperatureSensor7 = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(
            dataBuffer_, 212, 2));

    result.temperatureSensor8 = static_cast<unsigned short>(BufferUtils::GetNumberFromByteBufferLittleEndian(
            dataBuffer_, 214, 2));

    result.thermalManagementTemperature1TransitionCount = static_cast<unsigned int>(BufferUtils::GetNumberFromByteBufferLittleEndian(
            dataBuffer_, 216, 4));

    result.thermalManagementTemperature2TransitionCount = static_cast<unsigned int>(BufferUtils::GetNumberFromByteBufferLittleEndian(
            dataBuffer_, 220, 4));

    result.totalTimeForThermalManagementTemperature1 = static_cast<unsigned int>(BufferUtils::GetNumberFromByteBufferLittleEndian(
            dataBuffer_, 224, 4));

    result.totalTimeForThermalManagementTemperature2 = static_cast<unsigned int>(BufferUtils::GetNumberFromByteBufferLittleEndian(
            dataBuffer_, 228, 4));

    return result;

}

}



