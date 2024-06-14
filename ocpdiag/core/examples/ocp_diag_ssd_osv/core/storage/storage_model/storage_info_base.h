// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_STORAGE_INFO_BASE_H
#define OCP_DIAG_SSD_OSV_STORAGE_INFO_BASE_H

#include <string>
#include "ocpdiag/core/results/data_model/input_model.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_controller/storage_controller_base.h"

namespace ocp_diag_ssd_osv {

enum class StorageProtocol {
    Unknown,
    NVMe
};

// Abstract class for Storage Model
class StorageInfoBase {

public:
    StorageInfoBase() = default;
    StorageInfoBase(const std::string& devPath,
                        std::shared_ptr<StorageControllerBase> storageController, StorageProtocol storageProtocol) :
            devPath_(devPath), storageController_(storageController), storageProtocol_(storageProtocol) {}

    virtual ~StorageInfoBase() = default;

public:
    // Convert StorageInfo to ocpdiag::results::HardwareInfo
    ocpdiag::results::HardwareInfo                    To() const;

    // It holds ocpdiag::results::RegisteredHardwareInfo.
    void                                              SetRegisteredHardwareInfo(const ocpdiag::results::RegisteredHardwareInfo &registeredHardwareInfo) { registeredHardwareInfo_ = registeredHardwareInfo; }
    const ocpdiag::results::RegisteredHardwareInfo&   GetRegisteredHardwareInfoRef() const { return registeredHardwareInfo_; }

    const std::string&                                GetDevicePath() const { return devPath_; } // ex) /dev/nvme0
    std::shared_ptr<StorageControllerBase>            GetStorageController() { return storageController_; }
    StorageProtocol                                   GetStorageProtocol() { return storageProtocol_; }
    virtual RAIIHandle::handleValueType               CreateDeviceHandle();
    static RAIIHandle::handleValueType                CreateDeviceHandle(const std::string& devPath);

    bool                                              SendStorageCommand(RAIIHandle& handle,
                                                                         std::shared_ptr<StorageCommandBase> storageCommand);

    virtual std::string                               GetSerialNumber() const = 0;
    virtual std::string                               GetFwVersion() const = 0;
    virtual std::string                               GetModelNumber() const = 0;

protected:
    std::string                                       devPath_; // Device Path. ex) /dev/nvme0
    std::shared_ptr<StorageControllerBase>            storageController_; // Object for sending commands. It depends on environment(ex. Driver, OS, ...)
    ocpdiag::results::RegisteredHardwareInfo          registeredHardwareInfo_;
    StorageProtocol                                   storageProtocol_;
};

}

#endif //OCP_DIAG_SSD_OSV_STORAGE_INFO_BASE_H
