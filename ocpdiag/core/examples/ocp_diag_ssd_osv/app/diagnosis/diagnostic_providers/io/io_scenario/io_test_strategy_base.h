#ifndef FULL_SPEC_IO_TEST_STRATEGY_BASE_H
#define FULL_SPEC_IO_TEST_STRATEGY_BASE_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_model/storage_info_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/io/io_scenario/io_test_option.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/io/storage_nvme_io_command_base.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_result_log.h"
#include <memory>
#include <functional>

namespace ocp_diag_ssd_osv {

using IoFunc = std::function<std::shared_ptr<DiagnosticResultLog>(std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo,
                                                                  RAIIHandle& handle,
                                                                  unsigned long long startLBA,
                                                                  std::vector<unsigned char>& data)>;

class IoTestStrategyBase {

public:
    IoTestStrategyBase(std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo,
                       unsigned long long startLBA,
                       unsigned long long endLBAPlusOne, // 0 = Max LBA of Device
                       IoCmdRequestSize ioCmdRequestSize,
                       bool isWrite,
                       unsigned int targetNamespaceForNVMe = 1);

    virtual ~IoTestStrategyBase() = default;

public:
    std::vector<std::shared_ptr<DiagnosticLogBase>>             IoTest();
    bool                                                        IsWrite() { return isWrite_; }

protected:
    virtual std::shared_ptr<DiagnosticLogBase>                  ExecuteIoTest() = 0;
    virtual bool                                                UpdateParameters();
    virtual bool                                                ValidateParameters();
    virtual std::shared_ptr<DiagnosticLogBase>                  PostHandleDiagnosticResultLog(std::shared_ptr<DiagnosticLogBase>&& ioTestResult) { return ioTestResult; }

private:
    bool                                                        GenerateIoCommand();
    bool                                                        GenerateNVMeIoCommand();

    bool                                                        ValidateParametersForNVMe();

protected:
    std::vector<std::shared_ptr<DiagnosticLogBase>>             diagnosticResultLogs_;
    std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase>              storageInfo_;
    IoFunc                                                      ioFunc_;
    unsigned long long                                          startLBA_;
    unsigned long long                                          endLBAPlusOne_;
    IoCmdRequestSize                                            ioCmdRequestSize_;
    bool                                                        isWrite_;
    unsigned int                                                targetNamespaceForNVMe_;
    unsigned int                                                blockSize_ = 0;
    unsigned long long                                          deviceMaxLBA_ = 0;
    NVMeIdentifyNamespaceData                                   identifyNamespaceData_{};
};

}

#endif //FULL_SPEC_IO_TEST_STRATEGY_BASE_H
