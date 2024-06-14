#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/io/io_scenario/io_test_strategy_random_timebased.h"
#include <random>
#include <utility>

namespace ocp_diag_ssd_osv {

IoTestStrategyRandomTimebased::IoTestStrategyRandomTimebased(std::shared_ptr<StorageInfoBase> storageInfo,
                                                             unsigned long long startLBA,
                                                             IoCmdRequestSize ioCmdRequestSize,
                                                             unsigned long long endLBAPlusOne,
                                                             int testIoTimeSecond,
                                                             bool isWrite,
                                                             unsigned int targetNamespaceForNVMe) :

        IoTestStrategyBase(std::move(storageInfo), startLBA, endLBAPlusOne, ioCmdRequestSize, isWrite, targetNamespaceForNVMe),
        testIoTimeSecond_(testIoTimeSecond) {

}

std::shared_ptr<DiagnosticLogBase> IoTestStrategyRandomTimebased::ExecuteIoTest() {

    std::string testNameBaseStr = "random-io-test-for-" + std::to_string(testIoTimeSecond_) + "-seconds";

    RAIIHandle handle = storageInfo_->CreateDeviceHandle();
    std::vector<unsigned char> dataBuf;

    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937_64 gen(seed);

    unsigned int blockCount = (static_cast<unsigned int>(ioCmdRequestSize_) / blockSize_);

    time_t startTime = time(nullptr);
    std::uniform_int_distribution<unsigned long long> lbaDis(startLBA_, (endLBAPlusOne_ - blockCount - 1));

    while (true)
    {
        if (time(nullptr) - startTime >= testIoTimeSecond_) break;
        unsigned long long randomStartLba = lbaDis(gen);

        std::shared_ptr<DiagnosticResultLog> ioFuncResult = ioFunc_(storageInfo_, handle, randomStartLba, dataBuf);

        // I/O Command Fail
        if(ioFuncResult->GetDiagnosisResultType() == DiagnosisResultType::Fail)
        {
            return std::make_shared<DiagnosticResultLog>(storageInfo_,
                                                         ioFuncResult->GetDiagnosisResultType(),
                                                         testNameBaseStr + "-fail",
                                                         ioFuncResult->GetMessage());
        }
    }

    // Test Pass
    return std::make_shared<DiagnosticResultLog>(storageInfo_,
                                                 DiagnosisResultType::Pass,
                                                 testNameBaseStr + "-pass",
                                                 "Random I/O Test has been performed successfully.");
}

}


