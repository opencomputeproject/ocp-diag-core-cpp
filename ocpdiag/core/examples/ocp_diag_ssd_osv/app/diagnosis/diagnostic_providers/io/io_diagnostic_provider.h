// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_IO_DIAGNOSTIC_PROVIDER_H
#define OCP_DIAG_SSD_OSV_IO_DIAGNOSTIC_PROVIDER_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/storage_diagnostic_provider_interface.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/io/io_scenario/io_test_strategy_base.h"
#include <functional>
#include <vector>
#include <memory>

namespace ocp_diag_ssd_osv {

class IoDiagnosticProvider : public StorageDiagnosticProviderInterface {
public:
    explicit IoDiagnosticProvider(const IoTestOption& ioTestOption);
    ~IoDiagnosticProvider() override = default;

    std::vector<std::shared_ptr<DiagnosticLogBase>> Diagnose(std::shared_ptr<StorageInfoBase> storageInfo) override;
    std::string                                     GetDiagnosisName() override { return ioTestOption_.GenerateIoTestNameString(); }
    bool                                            Supports(std::shared_ptr<StorageInfoBase> storageInfo) override { return true; }

private:
    std::shared_ptr<IoTestStrategyBase>             GenerateIoTestStrategy(std::shared_ptr<StorageInfoBase> storageInfo);


protected:
    IoTestOption                                    ioTestOption_;
};

}


#endif //OCP_DIAG_SSD_OSV_IO_DIAGNOSTIC_PROVIDER_H
