// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_NVME_CHECK_SMART_DIAGNOSTIC_PROVIDER_H
#define OCP_DIAG_SSD_OSV_NVME_CHECK_SMART_DIAGNOSTIC_PROVIDER_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/storage_diagnostic_provider_interface.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_command_get_log_page_smart_health_info_data.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/ocp_diag_ssd_osv_test_properties.h"

namespace ocp_diag_ssd_osv {

// Class for diagnose SSD using NVMe SMART Data
class NVMeCheckSmartDiagnosticProvider : public StorageDiagnosticProviderInterface {
public:
    NVMeCheckSmartDiagnosticProvider(TestType testType) : testType_(testType) { }
    ~NVMeCheckSmartDiagnosticProvider() override = default;

    std::vector<std::shared_ptr<DiagnosticLogBase>> Diagnose(std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo) override;
    std::string                                     GetDiagnosisName() override { return "nvme-smart-check"; }
    bool                                            Supports(std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo) override;

protected:
    bool                                            ExecuteGetLogPageCommand(std::vector<std::shared_ptr<DiagnosticLogBase>>& result,
                                                                             NVMeSmartData& nvmeSmartDataResult,
                                                                             std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo);
    void                                            CheckCapFail(std::vector<std::shared_ptr<DiagnosticLogBase>>& result,
                                                                 const NVMeSmartData& nvmeSmartDataResult,
                                                                 std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo);
    void                                            CheckReadOnlyMode(std::vector<std::shared_ptr<DiagnosticLogBase>>& result,
                                                                      const NVMeSmartData& nvmeSmartDataResult,
                                                                      std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo);
    void                                            CheckAvailableSpare(std::vector<std::shared_ptr<DiagnosticLogBase>>& result,
                                                                        const NVMeSmartData& nvmeSmartDataResult,
                                                                        std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo);
    void                                            CheckPercentageUsed(std::vector<std::shared_ptr<DiagnosticLogBase>>& result,
                                                                               const NVMeSmartData& nvmeSmartDataResult,
                                                                               std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo);

protected:
    TestType                                        testType_;

};

}



#endif //OCP_DIAG_SSD_OSV_NVME_CHECK_SMART_DIAGNOSTIC_PROVIDER_H
