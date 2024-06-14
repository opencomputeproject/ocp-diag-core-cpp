// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/io/io_diagnostic_provider.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/io/io_scenario/io_test_strategy_random_timebased.h"

namespace ocp_diag_ssd_osv {

ocp_diag_ssd_osv::IoDiagnosticProvider::IoDiagnosticProvider(const ocp_diag_ssd_osv::IoTestOption& ioTestOption)
        : ioTestOption_(ioTestOption) {
}

std::vector<std::shared_ptr<DiagnosticLogBase>> IoDiagnosticProvider::Diagnose(std::shared_ptr<StorageInfoBase> storageInfo) {

    std::shared_ptr<IoTestStrategyBase> ioTestStrategy = GenerateIoTestStrategy(storageInfo);
    return ioTestStrategy->IoTest();
}

std::shared_ptr<IoTestStrategyBase> IoDiagnosticProvider::GenerateIoTestStrategy(std::shared_ptr<StorageInfoBase> storageInfo) {
    std::shared_ptr<IoTestStrategyBase> ret = nullptr;

    switch(ioTestOption_.GetIoTestType())
    {
        case IoTestType::RandomIo:
            switch(ioTestOption_.GetIoTestEndConditionType())
            {
                case IoTestEndConditionType::TimeBase:
                    ret = std::make_shared<IoTestStrategyRandomTimebased>(
                            storageInfo,
                            ioTestOption_.GetStartLba(),
                            ioTestOption_.GetIoCmdRequestSize(),
                            ioTestOption_.GetEndLbaPlusOne(),
                            ioTestOption_.GetTestIoTimeSecond(),
                            ioTestOption_.IsWrite());
                    break;
                case IoTestEndConditionType::IoAmountBase:
                    assert(false && "Not Implemented");
                    break;
            }
            break;
        case IoTestType::SequentialIo:
            assert(false && "Not Implemented");
            break;
        default:
            assert(false && "Invalid I/O Test Type");
    }

    return ret;
}

}

