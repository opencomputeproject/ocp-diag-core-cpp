// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_STORAGE_NVME_IO_COMMAND_BASE_H
#define OCP_DIAG_SSD_OSV_STORAGE_NVME_IO_COMMAND_BASE_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/storage_nvme_command_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_identify_namespace_data.h"

namespace ocp_diag_ssd_osv {

// Base class for NVMe I/O Commands(Read/Write)
class StorageNVMeIoCommandBase : public StorageNVMeCommandBase {
public:
    StorageNVMeIoCommandBase(NVMeCommandOpCode opCode,
                             const NVMeIdentifyNamespaceData& nvmeIdentifyNamespaceData,
                             unsigned long long startLBA,
                             unsigned int blockCount);

    ~StorageNVMeIoCommandBase() override = default;

    bool                                BeforeCommandExecutionHandler() override;

    size_t                              CalcTotalDataSizeInBytes() const { return GetTargetIoBlockCount() * nvmeIdentifyNamespaceData_.GetCurrentLbaFormat().GetLbaDataSize(); }
    size_t                              CalcTotalMetadataSizeInBytes() const { return GetTargetIoBlockCount() * nvmeIdentifyNamespaceData_.GetCurrentLbaFormat().metaDataSize; }
    bool                                IsMetadataEndOfLBA() const { return nvmeIdentifyNamespaceData_.IsMetadataAtEndOfLBA(); }

    StorageCommandType                  GetStorageCommandType() override { return StorageCommandType::NVMeIoCommand; }
    bool                                ResizeDataBufferWithGivenInfo();
    void                                SetStartLBA(unsigned long long startLba);
    unsigned long long                  GetStartLBA();
    void                                SetNVMeNamespaceDataInfo(const NVMeIdentifyNamespaceData& nvmeIdentifyNamespaceData);
    const NVMeIdentifyNamespaceData&    GetNVMeNamespaceDataInfoRef() const { return nvmeIdentifyNamespaceData_; }
    NVMeCommandOpCode                   GetNVMeCommandOpCode() const;

    void                                SetTargetIoBlockCount(unsigned int blockCount) ;
    unsigned int                        GetTargetIoBlockCount() const;
    virtual bool                        IsWriteCommand() = 0;

protected:
    NVMeIdentifyNamespaceData           nvmeIdentifyNamespaceData_;
};

}

#endif //OCP_DIAG_SSD_OSV_STORAGE_NVME_IO_COMMAND_BASE_H
