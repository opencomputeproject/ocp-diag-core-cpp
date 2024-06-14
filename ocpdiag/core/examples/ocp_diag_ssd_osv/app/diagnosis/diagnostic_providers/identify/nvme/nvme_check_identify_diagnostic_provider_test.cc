// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "gtest/gtest.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/identify/nvme/nvme_check_identify_diagnostic_provider.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_command_identify_controller.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_scanner/storage_fixture.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_controller/mock_nvme_storage_controller.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_result_log.h"

namespace ocp_diag_ssd_osv {

TEST(NVMeCheckIdentifyDiagnosisProviderTest, NVMeIdentifyControllerPassCase)
{
    // Create Mock Device
    std::shared_ptr<NVMeStorageInfo> mockNVMeDevice = StorageFixture::CreateMockNVMeDevice({"/dev/nvme0"});

    // Mock Storage Controller
    std::shared_ptr<MockNVMeStorageController> storageController
            = std::dynamic_pointer_cast<MockNVMeStorageController>(mockNVMeDevice->GetStorageController());
    EXPECT_CALL(*storageController, SendIdentifyControllerCommand)
            .Times(1)
            .WillOnce(testing::Invoke([](RAIIHandle &handle,
                                                                std::shared_ptr<StorageNVMeIdentifyControllerCommand> nvmeIdentifyControllerCommand) {
                std::shared_ptr<StorageNVMeCommandResult> nvmeCommandResult = nvmeIdentifyControllerCommand->GetStorageNVMeCommandResult();
                nvmeCommandResult->SetStatusType(NVMeStatusCodeType::GenericCommandStatus);
                nvmeCommandResult->SetStatusCode(NVMeGenericCommandStatusCode::InternalError);
            }));

    // Execute Diagnostic Provider
    NVMeCheckIdentifyDiagnosticProvider identifyDiagnosisProvider;
    ASSERT_TRUE(identifyDiagnosisProvider.Supports(mockNVMeDevice));

    // Validate Results
    std::vector<std::shared_ptr<DiagnosticLogBase>> diagnosticResult = identifyDiagnosisProvider.Diagnose(mockNVMeDevice);
    ASSERT_EQ(diagnosticResult.size(), 1);
    ASSERT_EQ(diagnosticResult[0]->GetStorageTestLogType(), StorageTestLogType::Diagnosis);

    const std::shared_ptr<DiagnosticResultLog> diagnosticResultLog = std::dynamic_pointer_cast<DiagnosticResultLog>(diagnosticResult[0]);
    ASSERT_NE(diagnosticResultLog, nullptr);
    ASSERT_EQ(diagnosticResultLog->GetDiagnosisResultType(), DiagnosisResultType::Fail);
    ASSERT_EQ(diagnosticResultLog->GetVerdict(), "nvme-identify-controller-command-fail");
    ASSERT_EQ(diagnosticResultLog->GetMessage(), "NVMe Identify Controller - Status Type(0),Status Code(Internal Error)");
}
    
}

