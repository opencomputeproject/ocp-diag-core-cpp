// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <map>
#include <sstream>
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/storage_command_result/nvme/storage_nvme_command_result.h"

namespace ocp_diag_ssd_osv {

bool StorageNVMeCommandResult::IsOK() const {

    // StorageCommandResultBase has higher priority
    if (StorageCommandResultBase::IsOK() == false)
        return false;

    // Then checks StorageNVMeCommandResult
    return IsStorageNVMeCmdResultOK();
}

bool StorageNVMeCommandResult::IsStorageNVMeCmdResultOK() const {
    return nvmeStatusType_ == NVMeStatusCodeType::GenericCommandStatus &&
           nvmeStatusCode_ == NVMeGenericCommandStatusCode::SuccessfulCompletion;
}

std::string StorageNVMeCommandResult::GetErrorCodeString() const {

    static std::map<NVMeStatusCodeType, std::map<unsigned int, std::string>> s_nvmeErrCode
            {
                    {NVMeStatusCodeType::GenericCommandStatus,
                            {
                                    {NVMeGenericCommandStatusCode::SuccessfulCompletion, "Successful Completion"},
                                    {NVMeGenericCommandStatusCode::InvalidCommandOpcode, "Invalid Opcode"},
                                    {NVMeGenericCommandStatusCode::InvalidFieldInCommand, "Invalid Field In Command"},
                                    {NVMeGenericCommandStatusCode::SRAMTestFailReturn, "SRAM Test Fail Return"},
                                    {NVMeGenericCommandStatusCode::CommandIdConflict, "Command Id Conflict"},
                                    {NVMeGenericCommandStatusCode::DataTransferError, "Data Transfer Error"},
                                    {NVMeGenericCommandStatusCode::CommandsAbortedDueToPowerLossNotification, "Command Aborted Due to Power Loss Notification"},
                                    {NVMeGenericCommandStatusCode::InternalError, "Internal Error"},
                                    {NVMeGenericCommandStatusCode::CommandAbortRequested, "Command Abort Requested"},
                                    {NVMeGenericCommandStatusCode::CommandAbortedDuetoSQDeletion, "Command Aborted Due to SQ Deletion"},
                                    {NVMeGenericCommandStatusCode::CommandAbortedDuetoFailedFusedCommand, "Command Aborted Due to Failed Fused Command"},
                                    {NVMeGenericCommandStatusCode::CommandAbortedDuetoMissingFusedCommand, "Command Aborted Due to Missing Fused Command"},
                                    {NVMeGenericCommandStatusCode::InvalidNamespaceOrFormat, "Invalid Namespace or Format"},
                                    {NVMeGenericCommandStatusCode::CommandSequenceError, "Command Sequence Error"},
                                    {NVMeGenericCommandStatusCode::InvalidSGLSegmentDescriptor, "Invalid SGL Segment Descriptor"},
                                    {NVMeGenericCommandStatusCode::InvalidNumberOfSGLDescriptors, "Invalid Number of SGL Descriptor"},
                                    {NVMeGenericCommandStatusCode::DataSGLLengthInvalid, "Data SGL Length Invalid"},
                                    {NVMeGenericCommandStatusCode::MetadataSGLLengthInvalid, "Meta Data SGL Length Invalid"},
                                    {NVMeGenericCommandStatusCode::SGLDescriptorTypeInvalid, "SGL Descriptor Type Invalid"},
                                    {NVMeGenericCommandStatusCode::InvalidUseOfControllerMemoryBuffer, "Invalid Use Of Controller Memory Buffer"},
                                    {NVMeGenericCommandStatusCode::PRPOffsetInvalid, "PRP Offset Invalid"},
                                    {NVMeGenericCommandStatusCode::AtomicWriteUnitExceeded, "Atomic Write Unit Exceeded"},

                                    {NVMeGenericCommandStatusCode::LBAOutOfRange, "LBA Out Of Range"},
                                    {NVMeGenericCommandStatusCode::CapacityExceeded, "Capacity Exceeded"},
                                    {NVMeGenericCommandStatusCode::NamespaceNotReady, "Namespace Not Ready"},
                                    {NVMeGenericCommandStatusCode::ReservationConflict, "Reservation Conflict"},
                                    {NVMeGenericCommandStatusCode::FormatInProgress, "Format In Progress"},
                            }
                    },

                    {NVMeStatusCodeType::CommandSpecificStatus,
                            {
                                    {NVMeCommandSpecificStatusCode::CompletionQueueInvalid, "Completion Queue Invalid"},
                                    {NVMeCommandSpecificStatusCode::InvalidQueueIdentifier, "Invalid Queue Identifier"},
                                    {NVMeCommandSpecificStatusCode::InvalidQueueSize, "Invalid Queue Size"},
                                    {NVMeCommandSpecificStatusCode::AbortQueueSize, "Abort Queue Size"},
                                    {NVMeCommandSpecificStatusCode::AsyncEventRequestLimitExceeded, "Async Event Request Limit Exceeded"},
                                    {NVMeCommandSpecificStatusCode::InvalidFirmwareSlot, "Invalid Firmware Slot"},
                                    {NVMeCommandSpecificStatusCode::InvalidFirmwareImage, "Invalid Firmware Image"},
                                    {NVMeCommandSpecificStatusCode::InvalidInterruptVector, "Invalid Interrupt Vector"},
                                    {NVMeCommandSpecificStatusCode::InvalidLogPage, "Invalid Log Page"},
                                    {NVMeCommandSpecificStatusCode::InvalidFormat, "Invalid Format"},
                                    {NVMeCommandSpecificStatusCode::FirmwareActivationRequiresConventionalReset, "Firmware Activation Requires Conven"},
                                    {NVMeCommandSpecificStatusCode::InvalidQueueDeletion, "Invalid Queue Deletion"},
                                    {NVMeCommandSpecificStatusCode::FeatureIdentifierNotSaveable, "Feature Identifier Not Saveable"},
                                    {NVMeCommandSpecificStatusCode::FeatureNotChangeable, "Feature Not Changeable"},
                                    {NVMeCommandSpecificStatusCode::FeatureNotNamespaceSpecific, "Feature Namespace Specific"},
                                    {NVMeCommandSpecificStatusCode::FirmwareActivationRequiresNVMSubsystemReset, "Firmware Activation Requires NVM Subsystem Reset"},
                                    {NVMeCommandSpecificStatusCode::FirmwareActivationRequiresReset, "Firmware Activation Requires Reset"},
                                    {NVMeCommandSpecificStatusCode::FirmwareActivationRequiresMaximumTimeViolation, "Firmware Activation Reuiqres Maximum Time Violation"},
                                    {NVMeCommandSpecificStatusCode::FirmwareActivationProhibited, "Firmware Activation Prohibited"},
                                    {NVMeCommandSpecificStatusCode::OverlappingRange, "Overlapping Range"},
                                    {NVMeCommandSpecificStatusCode::NamespaceInsufficientCapacity, "Namesapce Insufficient Capacity"},
                                    {NVMeCommandSpecificStatusCode::NamespaceIdentifierUnavailable, "Namespace Identifer Unavailable"},
                                    {NVMeCommandSpecificStatusCode::NamespaceAlreadyAttached, "Namespace Already Attached"},
                                    {NVMeCommandSpecificStatusCode::NamespaceIsPrivate, "Namespace is Private"},
                                    {NVMeCommandSpecificStatusCode::NamespaceNotAttached, "Namespace Not Attached"},
                                    {NVMeCommandSpecificStatusCode::ThinProvisioningNotSupported, "This Provisioning Not Supported"},
                                    {NVMeCommandSpecificStatusCode::ControllerListInvalid, "Controller List Invalid"},
                                    {NVMeCommandSpecificStatusCode::Device_Self_Test_In_Progress, "Device Self Test In Progress"},
                                    {NVMeCommandSpecificStatusCode::BootPartionWriteProhibited, "Boot Partition Write Prohibited"},
                                    {NVMeCommandSpecificStatusCode::InvalidControllerIdentifier, "Invalid Controller Identifier"},
                                    {NVMeCommandSpecificStatusCode::InvalidSecondaryControllerState, "Invalid Secondary Controller State"},
                                    {NVMeCommandSpecificStatusCode::InvalidNumberOfControllerResources, "Invalid Number Of Controller Resources"},
                                    {NVMeCommandSpecificStatusCode::InvalidResourceIdentifier, "Invalid Resource Identifier"},

                                    {NVMeCommandSpecificStatusCode::ConflictingAttributes, "Conflicting Attributes"},
                                    {NVMeCommandSpecificStatusCode::InvalidProtectionInformation, "Invalid Protection Information"},
                                    {NVMeCommandSpecificStatusCode::AttemptedWriteToReadOnlyRange, "Attempted Write ToOcpDiagDiagnosis Read Only Range"},
                            }
                    },

                    {NVMeStatusCodeType::MediaAndDataIntegrityErrors,
                            {
                                    {NVMeMediaAndDataIntegrityErrorStatusCode::WriteFault, "Write Fault"},
                                    {NVMeMediaAndDataIntegrityErrorStatusCode::UnrecoveredReadError, "Unrecovered Read Error"},
                                    {NVMeMediaAndDataIntegrityErrorStatusCode::EndToEndGuardCheckError, "End to End Guard Check Error"},
                                    {NVMeMediaAndDataIntegrityErrorStatusCode::EndToEndApplicationTagCheckError, "End to End Application Tag Check Error"},
                                    {NVMeMediaAndDataIntegrityErrorStatusCode::EndToEndReferenceTagCheckError, "End to End Reference Tag Check Error"},
                                    {NVMeMediaAndDataIntegrityErrorStatusCode::CompareFailure, "Compare Failure"},
                                    {NVMeMediaAndDataIntegrityErrorStatusCode::AccessDenied, "Access Denied"},
                                    {NVMeMediaAndDataIntegrityErrorStatusCode::DeallocatedOrUnwrittenLogicalBlock, "Deallocate or Unwritten Logical Block"},
                            }
                    }
            };

    // Only if NVMe cmd fail
    if(IsStorageNVMeCmdResultOK() == false)
    {
        std::stringstream ss;

        if (s_nvmeErrCode.find(nvmeStatusType_) == s_nvmeErrCode.end())
        {
            ss << "Unknown Status Type(" << static_cast<unsigned long long>(nvmeStatusType_) << "),";
            ss << "Status Code(" << static_cast<unsigned long long>(nvmeStatusCode_) << ")";
        }
        else
        {
            ss << "Status Type(" << static_cast<unsigned long long>(nvmeStatusType_) << "),";
            std::map<unsigned int, std::string>& currentStatusTypeErrCode = s_nvmeErrCode[nvmeStatusType_];

            if (currentStatusTypeErrCode.find(nvmeStatusCode_) == currentStatusTypeErrCode.end())
            {
                ss << "Unknown Status Code(" << nvmeStatusCode_ << ")";
            }
            else
            {
                ss << "Status Code(" << currentStatusTypeErrCode[nvmeStatusCode_] << ")";
            }
        }

        return ss.str();
    }

    // if not, use command result base
    return StorageCommandResultBase::GetErrorCodeString();
}

}

