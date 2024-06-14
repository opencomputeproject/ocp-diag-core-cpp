// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_STORAGE_NVME_COMMAND_GET_LOG_PAGE_SMART_HEALTH_INFO_DATA_H
#define OCP_DIAG_SSD_OSV_STORAGE_NVME_COMMAND_GET_LOG_PAGE_SMART_HEALTH_INFO_DATA_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/utils/BigInt.hpp"

namespace ocp_diag_ssd_osv {

struct NVMeSmartData {

    // Bytes 0 Critical Warning
    unsigned char			criticalWarning = 0;

    // Bytes 2:1 Composite Temperature
    unsigned short			compositeTemperature = 0;

    // Bytes 3 Available Spare
    unsigned char			availableSpare = 0;

    // Bytes 4 Available Spare Threshold
    unsigned char			availableSpareThreshold = 0;

    // Bytes 5 Percentage Used
    unsigned char			percentageUsed = 0;

    // Bytes 47:32 Data Units Read
    BigInt			        dataUnitsRead = 0;

    // Bytes 63:48 Data Units Written
    BigInt			        dataUnitsWritten = 0;

    // Bytes 79:64 Host Read Commands
    BigInt			        hostReadCommands = 0;

    // Bytes 95:80 Host Write Commands
    BigInt			        hostWriteCommands = 0;

    // Bytes 111:96 Controller Busy Time
    BigInt			        controllerBusyTime = 0;

    // Bytes 127:112 Power Cycles
    BigInt			        powerCycles = 0;

    // Bytes 143:128 Power On Hours
    BigInt			        powerOnHours = 0;

    // Bytes 159:144 Unsafe Shutdowns
    BigInt			        unsafeShutdowns = 0;

    // Bytes 175:160 Media and Data Integrity Errors
    BigInt			        mediaAndDataIntegrityErrors = 0;

    // Bytes 191:176 Number of Error Information Log Entries
    BigInt			        numberOfErrorInformationLogEntries = 0;

    // Bytes 195:192 Warning Composite Temperature Time
    unsigned int			warningCompositeTemperatureTime = 0;

    // Bytes 199:196 Critical Composite Temperature Time
    unsigned int			criticalCompositeTemperatureTime = 0;

    // Bytes 201:200 Temperature Sensor 1
    unsigned short			temperatureSensor1 = 0;

    // Bytes 203:202 Temperature Sensor 2
    unsigned short			temperatureSensor2 = 0;

    // Bytes 205:204 Temperature Sensor 3
    unsigned short			temperatureSensor3 = 0;

    // Bytes 207:206 Temperature Sensor 4
    unsigned short			temperatureSensor4 = 0;

    // Bytes 209:208 Temperature Sensor 5
    unsigned short			temperatureSensor5 = 0;

    // Bytes 211:210 Temperature Sensor 6
    unsigned short			temperatureSensor6 = 0;

    // Bytes 213:212 Temperature Sensor 7
    unsigned short			temperatureSensor7 = 0;

    // Bytes 215:214 Temperature Sensor 8
    unsigned short			temperatureSensor8 = 0;

    // Bytes 219:216 Thermal Management Temperature 1 Transition Count
    unsigned int			thermalManagementTemperature1TransitionCount = 0;

    // Bytes 223:220 Thermal Management Temperature 2 Transition Count
    unsigned int			thermalManagementTemperature2TransitionCount = 0;

    // Bytes 227:224 Total Time For Thermal Management Temperature 1
    unsigned int			totalTimeForThermalManagementTemperature1 = 0;

    // Bytes 231:228 Total Time For Thermal Management Temperature 2
    unsigned int			totalTimeForThermalManagementTemperature2 = 0;

};

}

#endif //OCP_DIAG_SSD_OSV_STORAGE_NVME_COMMAND_GET_LOG_PAGE_SMART_HEALTH_INFO_DATA_H
