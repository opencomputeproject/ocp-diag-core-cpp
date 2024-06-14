// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_STORAGE_NVME_COMMAND_RESULT_H
#define OCP_DIAG_SSD_OSV_STORAGE_NVME_COMMAND_RESULT_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/storage_command_result/storage_command_result_base.h"

namespace ocp_diag_ssd_osv {

enum NVMeStatusCodeType
{
    GenericCommandStatus = 0x0,
    CommandSpecificStatus = 0x1,
    MediaAndDataIntegrityErrors = 0x2,
    VendorSpecific = 0x7,
};

enum NVMeGenericCommandStatusCode
{
    SuccessfulCompletion = 0x00,
    InvalidCommandOpcode = 0x01,
    InvalidFieldInCommand = 0x02,
    SRAMTestFailReturn = 0x02,
    CommandIdConflict = 0x03,
    DataTransferError = 0x04,
    CommandsAbortedDueToPowerLossNotification = 0x05,
    InternalError = 0x06,
    CommandAbortRequested = 0x07,
    CommandAbortedDuetoSQDeletion = 0x08,
    CommandAbortedDuetoFailedFusedCommand = 0x09,
    CommandAbortedDuetoMissingFusedCommand = 0x0A,
    InvalidNamespaceOrFormat = 0x0B,
    CommandSequenceError = 0x0C,
    InvalidSGLSegmentDescriptor = 0x0D,
    InvalidNumberOfSGLDescriptors = 0x0E,
    DataSGLLengthInvalid = 0x0F,
    MetadataSGLLengthInvalid = 0x10,
    SGLDescriptorTypeInvalid = 0x11,
    InvalidUseOfControllerMemoryBuffer = 0x12,
    PRPOffsetInvalid = 0x13,
    AtomicWriteUnitExceeded = 0x14,

    LBAOutOfRange = 0x80,
    CapacityExceeded = 0x81,
    NamespaceNotReady = 0x82,
    ReservationConflict = 0x83,
    FormatInProgress = 0x84,
};

enum NVMeCommandSpecificStatusCode
{
    CompletionQueueInvalid = 0x00,
    InvalidQueueIdentifier = 0x01,
    InvalidQueueSize = 0x02,
    AbortQueueSize = 0x03,
    AsyncEventRequestLimitExceeded = 0x05,
    InvalidFirmwareSlot = 0x06,
    InvalidFirmwareImage = 0x07,
    InvalidInterruptVector = 0x08,
    InvalidLogPage = 0x09,
    InvalidFormat = 0x0A,
    FirmwareActivationRequiresConventionalReset = 0x0B,
    InvalidQueueDeletion = 0x0C,
    FeatureIdentifierNotSaveable = 0x0D,
    FeatureNotChangeable = 0x0E,
    FeatureNotNamespaceSpecific = 0x0F,
    FirmwareActivationRequiresNVMSubsystemReset = 0x10,
    FirmwareActivationRequiresReset = 0x11,
    FirmwareActivationRequiresMaximumTimeViolation = 0x12,
    FirmwareActivationProhibited = 0x13,
    OverlappingRange = 0x14,
    NamespaceInsufficientCapacity = 0x15,
    NamespaceIdentifierUnavailable = 0x16,
    NamespaceAlreadyAttached = 0x18,
    NamespaceIsPrivate = 0x19,
    NamespaceNotAttached = 0x1A,
    ThinProvisioningNotSupported = 0x1B,
    ControllerListInvalid = 0x1C,
    Device_Self_Test_In_Progress = 0x1D,
    BootPartionWriteProhibited = 0x1E,
    InvalidControllerIdentifier = 0x1F,
    InvalidSecondaryControllerState = 0x20,
    InvalidNumberOfControllerResources = 0x21,
    InvalidResourceIdentifier = 0x22,
    ConflictingAttributes = 0x80,
    InvalidProtectionInformation = 0x81,
    AttemptedWriteToReadOnlyRange = 0x82,
};

enum NVMeMediaAndDataIntegrityErrorStatusCode
{
    WriteFault = 0x80,
    UnrecoveredReadError = 0x81,
    EndToEndGuardCheckError = 0x82,
    EndToEndApplicationTagCheckError = 0x83,
    EndToEndReferenceTagCheckError = 0x84,
    CompareFailure = 0x85,
    AccessDenied = 0x86,
    DeallocatedOrUnwrittenLogicalBlock = 0x87,
};

class StorageNVMeCommandResult : public StorageCommandResultBase {
public:
    explicit StorageNVMeCommandResult(const std::string& cmdName) : StorageCommandResultBase(cmdName) {
    }

    StorageNVMeCommandResult(const std::string& cmdName, CommonErrorCode commonErrorCode) : StorageCommandResultBase(cmdName, commonErrorCode) {
    }

    ~StorageNVMeCommandResult() override = default;

    bool                                IsOK() const override;
    NVMeStatusCodeType	                GetStatusType() const { return nvmeStatusType_; }
    CommandResultType                   GetCommandResultType() override { return CommandResultType::NVMe; }
    void				                SetStatusType(NVMeStatusCodeType type) { nvmeStatusType_ = type; }

    unsigned int				        GetStatusCode() const { return nvmeStatusCode_; }
    void				                SetStatusCode(unsigned int statusCode) { nvmeStatusCode_ = statusCode; }
    std::string                         GetErrorCodeString() const override;

protected:
    // It returns NVMe command result is successful or not.
    bool				                IsStorageNVMeCmdResultOK()  const;

protected:
    NVMeStatusCodeType	                nvmeStatusType_ = NVMeStatusCodeType::GenericCommandStatus;
    unsigned int    	                nvmeStatusCode_ = NVMeGenericCommandStatusCode::SuccessfulCompletion;
};

}

#endif //OCP_DIAG_SSD_OSV_STORAGE_NVME_COMMAND_RESULT_H
