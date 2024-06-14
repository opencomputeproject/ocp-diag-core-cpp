#include <sstream>
#include <cassert>
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/io/io_scenario/io_test_option.h"

namespace ocp_diag_ssd_osv {

IoTestOption IoTestOption::CreateTimeBasedRandomIoTestOption(unsigned long long startLBA,
                                                              unsigned long long endLBAPlusOne,
                                                              ocp_diag_ssd_osv::IoCmdRequestSize ioCmdRequestSize,
                                                              int testIoTimeSecond,
                                                              bool isWrite,
                                                              unsigned int targetNVMeNamespace) {
    ocp_diag_ssd_osv::IoTestOption ioTestOption{};
    ioTestOption.startLBA_ = startLBA;
    ioTestOption.endLBAPlusOne_ = endLBAPlusOne;
    ioTestOption.ioTestType_ = IoTestType::RandomIo;
    ioTestOption.ioTestEndConditionType_ = IoTestEndConditionType::TimeBase;
    ioTestOption.ioCmdRequestSize_ = ioCmdRequestSize;
    ioTestOption.testIoTimeSecond_ = testIoTimeSecond;
    ioTestOption.isWrite_ = isWrite;
    ioTestOption.targetNVMeNamespace_ = targetNVMeNamespace;
    return ioTestOption;
}

std::string IoTestOption::GenerateIoTestNameString() {

    std::string randomOrSeqStr = (ioTestType_ == IoTestType::RandomIo ? "random" : (ioTestType_ == IoTestType::SequentialIo ? "sequential" : "unknown"));
    std::string readOrWriteStr = isWrite_ ? "write" : "read";
    std::string ioSizeStr;
    std::string testEndConditionStr;

    switch(ioCmdRequestSize_)
    {
        case IoCmdRequestSize::IO_SIZE_4KB:
            ioSizeStr = "4k";
            break;
        case IoCmdRequestSize::IO_SIZE_128KB:
            ioSizeStr = "128k";
            break;
    }

    switch(ioTestEndConditionType_)
    {
        case IoTestEndConditionType::TimeBase:
            testEndConditionStr = "for " + std::to_string(testIoTimeSecond_) + " seconds";
            break;
        case IoTestEndConditionType::IoAmountBase:
            testEndConditionStr = "I/O Amount : " + std::to_string(static_cast<double>(testIoAmountInBytes_) / 1024 / 1024 / 1024) + " GB";
            // TBDsd
            break;
    }

    std::stringstream ss;
    ss << randomOrSeqStr << "-" << readOrWriteStr;

    if(!ioSizeStr.empty())
        ss << "-" << ioSizeStr;

    if(!testEndConditionStr.empty())
        ss << "(" << testEndConditionStr << ")";

    return ss.str();
}

}

