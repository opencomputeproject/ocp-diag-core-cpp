#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/io/io_scenario/io_test_strategy_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/io/storage_nvme_read_command.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_command_identify_namespace.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_error_log.h"
#include <sstream>

namespace ocp_diag_ssd_osv {

IoTestStrategyBase::IoTestStrategyBase(std::shared_ptr<StorageInfoBase> storageInfo,
                                       unsigned long long startLBA,
                                       unsigned long long endLBAPlusOne,
                                       IoCmdRequestSize ioCmdRequestSize,
                                       bool isWrite,
                                       unsigned int targetNamespaceForNVMe) :
        storageInfo_(storageInfo), startLBA_(startLBA), endLBAPlusOne_(endLBAPlusOne), ioCmdRequestSize_(ioCmdRequestSize),
        isWrite_(isWrite), targetNamespaceForNVMe_(targetNamespaceForNVMe) {
}

std::vector<std::shared_ptr<DiagnosticLogBase>> IoTestStrategyBase::IoTest() {

    if(!UpdateParameters())
        return diagnosticResultLogs_;

    if(!ValidateParameters())
        return diagnosticResultLogs_;

    if(!GenerateIoCommand())
        return diagnosticResultLogs_;

    diagnosticResultLogs_.push_back(PostHandleDiagnosticResultLog(ExecuteIoTest()));
    return diagnosticResultLogs_;

}

bool IoTestStrategyBase::UpdateParameters() {

    RAIIHandle handle = storageInfo_->CreateDeviceHandle();
    switch(storageInfo_->GetStorageProtocol())
    {
        case ocp_diag_ssd_osv::StorageProtocol::NVMe:
        {
            if(storageInfo_->GetStorageProtocol() == ocp_diag_ssd_osv::StorageProtocol::NVMe)
            {
                std::shared_ptr<StorageNVMeIdentifyNamespaceCommand> idNamespaceCmd =
                        std::make_shared<StorageNVMeIdentifyNamespaceCommand>(targetNamespaceForNVMe_);
                if(storageInfo_->SendStorageCommand(handle, idNamespaceCmd) == false)
                {
                    diagnosticResultLogs_.push_back(
                            std::make_shared<DiagnosticResultLog>(storageInfo_,
                                                                  DiagnosisResultType::Fail,
                                                                  "nvme-identify-namespace-command-fail",
                                                                  idNamespaceCmd->GetStorageCommandResult()->GetCommandNameAndErrorCodeString()));
                    return false;
                }

                diagnosticResultLogs_.push_back(
                        std::make_shared<DiagnosticResultLog>(storageInfo_,
                                                              DiagnosisResultType::Pass,
                                                              "nvme-identify-namespace-command-pass",
                                                              "NVMe Identify Namespace command has been sent successfully."));

                identifyNamespaceData_ = idNamespaceCmd->GetParsedData();
                blockSize_ = identifyNamespaceData_.GetCurrentLbaFormat().GetLbaDataSize();
                deviceMaxLBA_ = identifyNamespaceData_.ncap;
            }
            break;
        }
        default:
            diagnosticResultLogs_.push_back(std::make_shared<DiagnosticErrorLog>(storageInfo_,
                                                                                 "Parameter Error",
                                                                                 "Invalid StorageProtocol"));
            return false;
            break;
    }

    // If EndLBA+1 is zero, set it to Device Max LBA.
    if(endLBAPlusOne_ == 0) {
        endLBAPlusOne_ = deviceMaxLBA_;
    }
    return true;
}

bool IoTestStrategyBase::ValidateParameters() {

    if(storageInfo_->GetStorageProtocol() == StorageProtocol::NVMe) {
        if(!ValidateParametersForNVMe())
            return false;
    }

    if(blockSize_ == 0) {
        diagnosticResultLogs_.push_back(std::make_shared<DiagnosticErrorLog>(storageInfo_,
                                                                             "Parameter Error",
                                                                             "blockSize_ must not be 0."));
        return false;
    }

    if (deviceMaxLBA_ == 0) {
        diagnosticResultLogs_.push_back(std::make_shared<DiagnosticErrorLog>(storageInfo_,
                                                                             "Parameter Error",
                                                                             "deviceMaxLBA_ must not be 0."));
        return false;
    }

    if(startLBA_ >= endLBAPlusOne_)
    {
        diagnosticResultLogs_.push_back(std::make_shared<DiagnosticErrorLog>(storageInfo_,
                                                                             "Parameter Error",
                                                                             "StartLBA must be lower than EndLBA."));
        return false;
    }

    unsigned int chunkBlockCount = static_cast<unsigned int>(ioCmdRequestSize_) / blockSize_;
    if(deviceMaxLBA_ <= (startLBA_ + chunkBlockCount))
    {
        std::stringstream ss;
        ss << "Test area(start LBA + block count = " << (startLBA_ + chunkBlockCount) << ") exceeds device capacity(" << deviceMaxLBA_ << ")";

        diagnosticResultLogs_.push_back(std::make_shared<DiagnosticErrorLog>(storageInfo_,
                                                                             "Parameter Error",
                                                                             ss.str()));
        return false;
    }

    return true;
}

bool IoTestStrategyBase::GenerateIoCommand() {

    switch(storageInfo_->GetStorageProtocol())
    {
        case StorageProtocol::NVMe:
            return GenerateNVMeIoCommand();
        default:
            diagnosticResultLogs_.push_back(std::make_shared<DiagnosticErrorLog>(storageInfo_,
                                                                                 "Parameter Error",
                                                                                 "Invalid StorageProtocol"));
            return false;
    }

}

bool IoTestStrategyBase::GenerateNVMeIoCommand() {

    unsigned int blockCount = (static_cast<unsigned int>(ioCmdRequestSize_) / blockSize_);

    ioFunc_ = [&identifyNamespaceData = identifyNamespaceData_,
               &diagnosticResultLogs = diagnosticResultLogs_,
            isWrite = isWrite_,
            blockCount = blockCount](std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo,
                                                RAIIHandle& handle,
                                                unsigned long long startLBA,
                                                std::vector<unsigned char>& data) {

        std::shared_ptr<ocp_diag_ssd_osv::StorageNVMeIoCommandBase> ioCmd = nullptr;
        if(isWrite == false)
        {
            ioCmd = std::make_shared<StorageNVMeReadCommand>(identifyNamespaceData,
                                                             startLBA,
                                                             blockCount);
        }
        else
        {
            diagnosticResultLogs.push_back(std::make_shared<DiagnosticErrorLog>(storageInfo,
                                                                                 "Parameter Error",
                                                                                 "Invalid StorageProtocol"));
        }

        std::shared_ptr<DiagnosticResultLog> diagnosticResultLog;

        if(storageInfo->SendStorageCommand(handle, ioCmd))
        {
           return std::make_shared<DiagnosticResultLog>(storageInfo,
                                           DiagnosisResultType::Pass,
                                           "nvme-io-command-pass",
                                           "NVMe I/O command has been sent successfully.");
        }
        else
        {
            return std::make_shared<DiagnosticResultLog>(storageInfo,
                                                         DiagnosisResultType::Fail,
                                                         "nvme-io-command-fail",
                                                         ioCmd->GetStorageCommandResult()->GetCommandNameAndErrorCodeString());
        }
    };

    return true;

}

bool IoTestStrategyBase::ValidateParametersForNVMe() {

    if(storageInfo_->GetStorageController()->GetStorageControllerType() == StorageControllerType::LinuxInbox) {
        if(identifyNamespaceData_.GetCurrentLbaFormat().metaDataSize > 0 &&
           identifyNamespaceData_.IsMetadataAtEndOfLBA() == false) {
            diagnosticResultLogs_.push_back(
                    std::make_shared<DiagnosticErrorLog>(storageInfo_,
                                                         "Metadata is not extended LBA",
                                                         "Metadata with not Extended LBA is not supported in Linux Inbox Driver."));

            return false;
        }
    }

    if(identifyNamespaceData_.isActiveNameSpace == false) {
        diagnosticResultLogs_.push_back(
                std::make_shared<DiagnosticErrorLog>(storageInfo_,
                                                     "Not Active NVMe Namespace",
                                                     std::string("Given namespace #") +
                                                        std::to_string(static_cast<long long>(identifyNamespaceData_.namespaceId)) + " is not active."));
        return false;
    }

    return true;
}

}


