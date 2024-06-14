// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <sstream>
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/storage_command_result/storage_command_result_base.h"

namespace ocp_diag_ssd_osv {

std::string StorageCommandResultBase::GetErrorCodeString() const {
    switch(GetCommonErrorCode())
    {
        case CommonErrorCode::None:
            return "None";
        case CommonErrorCode::LogicError:
            return "Logic Error";
        case CommonErrorCode::Timeout:
            return "Timeout";
        case CommonErrorCode::DriverStatusError:
            return "Driver Status Error";
        case CommonErrorCode::HostStatusError:
            return "Host Status Error";
        case CommonErrorCode::IoctlFail:
            return "Ioctl Fail";
        case CommonErrorCode::InvalidHandle:
            return "Invalid Handle";
        case CommonErrorCode::ResultIsAllZero:
            return "Result is All Zero";
        case CommonErrorCode::UnexpectedData:
            return "Unexpected Data";
        case CommonErrorCode::NotImplemented:
            return "Not Implemented";
        case CommonErrorCode::NotSupported:
            return "Not Supported";
        default:
            return "Undefined";
    }
}

std::string StorageCommandResultBase::GetCommandNameAndErrorCodeString() const {
    return cmdName_ + " - " + GetErrorCodeString();
}

}


