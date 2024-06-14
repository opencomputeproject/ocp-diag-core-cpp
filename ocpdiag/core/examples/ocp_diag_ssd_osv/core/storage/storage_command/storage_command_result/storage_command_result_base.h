// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_STORAGE_COMMAND_RESULT_BASE_H
#define OCP_DIAG_SSD_OSV_STORAGE_COMMAND_RESULT_BASE_H

#include <string>

namespace ocp_diag_ssd_osv {

// It describes what kind of protocol response
enum class CommandResultType
{
    Undefined,
    NVMe,
};

// It describes common error code which is not depends on a specific protocol(NVMe/SCSI ...).
enum class CommonErrorCode {
    None,
    LogicError,
    Timeout,
    DriverStatusError,
    HostStatusError,
    IoctlFail,
    InvalidHandle,
    ResultIsAllZero,
    UnexpectedData,
    NotImplemented,
    NotSupported,
};

// Class for describing result of storage command.
class StorageCommandResultBase {
public:
    explicit StorageCommandResultBase(const std::string& cmdName) : cmdName_(cmdName) {
    }

    StorageCommandResultBase(const std::string& cmdName, CommonErrorCode commonErrorCode) : StorageCommandResultBase(cmdName) {
        commonErrorCode_ = commonErrorCode;
    }

    virtual ~StorageCommandResultBase() = default;

    // It returns command result is successful or not.
    virtual bool			        IsOK() const { return commonErrorCode_ == CommonErrorCode::None; }

    virtual CommandResultType       GetCommandResultType() = 0;

    void                            SetCommonErrorCode(CommonErrorCode errorCode) { commonErrorCode_ = errorCode; }
    CommonErrorCode                 GetCommonErrorCode() const { return commonErrorCode_; }

    // It returns ioctl errno.
    unsigned long long              GetIoctlErrorCode() const { return ioctlErrorCode_; }
    void                            SetIoctlErrorCode(unsigned long long ioctlErrorCode) { ioctlErrorCode_ = ioctlErrorCode; }

    virtual std::string             GetErrorCodeString() const;
    std::string                     GetCommandNameAndErrorCodeString() const;

protected:
    CommonErrorCode                 commonErrorCode_ = CommonErrorCode::None;
    std::string                     cmdName_;            // Storage Command Name
    unsigned long long              ioctlErrorCode_ = 0; // errno
};

}

#endif //OCP_DIAG_SSD_OSV_STORAGE_COMMAND_RESULT_BASE_H
