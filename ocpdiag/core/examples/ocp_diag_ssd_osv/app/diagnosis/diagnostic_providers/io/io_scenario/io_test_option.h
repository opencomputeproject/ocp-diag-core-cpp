#ifndef FULL_SPEC_IO_TEST_OPTION_H
#define FULL_SPEC_IO_TEST_OPTION_H

#include <memory>
#include <limits>

namespace ocp_diag_ssd_osv {

// Low value is high priority
enum class IoTestType
{
    RandomIo,
    SequentialIo,
};

// Low value is high priority
enum class IoTestEndConditionType
{
    TimeBase,
    IoAmountBase,
};

enum class IoCmdRequestSize : unsigned int
{
    IO_SIZE_4KB = 4 * 1024,
    IO_SIZE_128KB = 128 * 1024,
};

class IoTestOption {
private:
    IoTestOption() = default;

public:
    static IoTestOption                     CreateTimeBasedRandomIoTestOption(unsigned long long startLBA,
                                                                              unsigned long long endLBAPlusOne, // 0 = Device Max LBA
                                                                              ocp_diag_ssd_osv::IoCmdRequestSize ioCmdRequestSize,
                                                                              int testIoTimeSecond,
                                                                              bool isWrite,
                                                                              unsigned int targetNVMeNamespace = 1);

    std::string                             GenerateIoTestNameString();

public:
    IoTestType                              GetIoTestType() const { return ioTestType_; }
    IoTestEndConditionType                  GetIoTestEndConditionType() const { return ioTestEndConditionType_; }
    unsigned long long                      GetStartLba() const { return startLBA_; }
    unsigned long long                      GetEndLbaPlusOne() const { return endLBAPlusOne_; }
    IoCmdRequestSize                        GetIoCmdRequestSize() const { return ioCmdRequestSize_; }
    unsigned long long                      GetTestIoAmountInBytes() const { return testIoAmountInBytes_; }
    int                                     GetTestIoTimeSecond() const { return testIoTimeSecond_; }
    bool                                    IsWrite() const { return isWrite_; }
    unsigned int                            GetTargetNVMeNamespace() const { return targetNVMeNamespace_; }

private:
    IoTestType                              ioTestType_ = IoTestType::RandomIo;
    IoTestEndConditionType                  ioTestEndConditionType_ = IoTestEndConditionType::TimeBase;
    unsigned long long                      startLBA_ = 0;
    unsigned long long                      endLBAPlusOne_ = 0;
    IoCmdRequestSize                        ioCmdRequestSize_ = IoCmdRequestSize::IO_SIZE_4KB;
    unsigned long long                      testIoAmountInBytes_ = 0;
    int                                     testIoTimeSecond_ = 0;
    bool                                    isWrite_ = false;
    unsigned int                            targetNVMeNamespace_{};

};

}


#endif //FULL_SPEC_IO_TEST_OPTION_H
