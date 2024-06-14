#ifndef FULL_SPEC_IO_TEST_STRATEGY_RANDOM_TIMEBASED_H
#define FULL_SPEC_IO_TEST_STRATEGY_RANDOM_TIMEBASED_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/io/io_scenario/io_test_strategy_base.h"

namespace ocp_diag_ssd_osv {

class IoTestStrategyRandomTimebased : public IoTestStrategyBase {
public:
    IoTestStrategyRandomTimebased(std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo,
                                  unsigned long long startLBA,
                                  IoCmdRequestSize ioCmdRequestSize,
                                  unsigned long long endLBAPlusOne, // 0 = Max LBA of Device
                                  int testIoTimeSecond,
                                  bool isWrite,
                                  unsigned int targetNamespaceForNVMe = 1);

    ~IoTestStrategyRandomTimebased() override = default;


protected:
    std::shared_ptr<DiagnosticLogBase>          ExecuteIoTest() override;

protected:
    int                                         testIoTimeSecond_;
};

}

#endif //FULL_SPEC_IO_TEST_STRATEGY_RANDOM_TIMEBASED_H
