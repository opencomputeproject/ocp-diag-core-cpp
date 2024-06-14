// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_APP_H
#define OCP_DIAG_SSD_OSV_APP_H

#include "ocpdiag/core/results/artifact_writer.h"
#include "ocpdiag/core/results/test_run.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_model/nvme_storage_info.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/ocp_diag_ssd_osv_test_properties.h"

namespace ocp_diag_ssd_osv {

using ocpdiag::results::DutInfo;

class OcpDiagSsdApp {
public:
    explicit OcpDiagSsdApp(std::unique_ptr<ocpdiag::results::internal::ArtifactWriter> writer = nullptr);

public:
    void                                                                    ExecuteTest(const std::vector<char*>& commandLine);
    ocpdiag::results::TestRun&                                              GetOcpDiagTestRunRef() { return run_; }

private:
    virtual std::vector<std::shared_ptr<StorageInfoBase>>                   ScanStorages();

    bool                                                                    CheckIsRoot();
    bool                                                                    UpdateOcpDiagSsdTestProperties(const std::vector<char*>& commandLine);
    bool                                                                    CreateDutInfo();
    void                                                                    RunDiagnostic();

    OcpDiagSsdTestProperties                                                ocpDiagSsdTestProperties_;
    ocpdiag::results::TestRun                                               run_;
    std::vector<std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase>>         targetStorageDevices_; // Only supports NVMe now
};

} // namespace ocp_diag_ssd_osv


#endif // OCP_DIAG_SSD_OSV_APP_H
