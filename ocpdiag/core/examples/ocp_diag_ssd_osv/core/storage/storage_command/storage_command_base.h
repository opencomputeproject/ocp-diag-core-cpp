// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_STORAGE_COMMAND_BASE_H
#define OCP_DIAG_SSD_OSV_STORAGE_COMMAND_BASE_H

#include <utility>
#include <vector>
#include <memory>
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/utils/RAIIHandle.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/storage_command_result/storage_command_result_base.h"

namespace ocp_diag_ssd_osv {

enum class StorageCommandType
{
    Unknown,
    NVMeAdminCommand,
    NVMeNonIoCommand,
    NVMeIoCommand
};

// Base class for All Storage Commands.
class StorageCommandBase : public std::enable_shared_from_this<StorageCommandBase> {
protected:
    enum : unsigned int
    {
        DEFAULT_COMMAND_TIMEOUT_VALUE = 15,
    };

public:
    StorageCommandBase() = default;
    virtual ~StorageCommandBase() = default;

    virtual std::string                         GetCommandName() = 0;
    virtual StorageCommandType                  GetStorageCommandType() = 0;

    // It will be called before command execution. It tells Storage Controller to further process the request (true) or not (false).
    virtual bool                                BeforeCommandExecutionHandler() { return true; }

    // It will be called after command execution.
    virtual void                                AfterCommandExecutionHandler() { }

    // It returns true if command is successful.
    bool                                        IsOK();

    void                                        SetStorageCommandResult(std::shared_ptr<StorageCommandResultBase> commandResult) { commandResult_ = std::move(commandResult); }
    std::shared_ptr<StorageCommandResultBase>	GetStorageCommandResult() { return commandResult_; }

    std::vector<unsigned char>&                 GetDataBufferRef() { return dataBuffer_; }

    bool                                        HasData() { return !dataBuffer_.empty(); }

    // command timeout
    void                                        SetTimeoutInSecond(unsigned int timeoutInSecond) { timeoutInSecond_ = timeoutInSecond; }
    unsigned int                                GetTimeoutInSecond() const { return timeoutInSecond_; }

protected:
    std::shared_ptr<StorageCommandResultBase>	commandResult_ = nullptr;
    unsigned int						        timeoutInSecond_ = DEFAULT_COMMAND_TIMEOUT_VALUE;
    std::vector<unsigned char>                  dataBuffer_;
};

}

#endif //OCP_DIAG_SSD_OSV_STORAGE_COMMAND_BASE_H
