// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "gtest/gtest.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_scanner/storage_fixture.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_controller/mock_nvme_storage_controller.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_result_log.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/smart/nvme/nvme_check_smart_diagnostic_provider.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_measurement_log.h"
#include <cassert>

namespace ocp_diag_ssd_osv {

TEST(NVMeCheckSmartDiagnosisProviderTest, GetLogPageCommandFailCaseForInitialAndFieldTest) {
    std::vector<TestType> testTypes = {TestType::Initial, TestType::Field};

    // Create Mock Device
    std::shared_ptr<NVMeStorageInfo> mockNVMeDevice = StorageFixture::CreateMockNVMeDevice();

    // Mock Storage Controller
    std::shared_ptr<MockNVMeStorageController> storageController
            = std::dynamic_pointer_cast<MockNVMeStorageController>(mockNVMeDevice->GetStorageController());
    EXPECT_CALL(*storageController, SendGetLogPageSmartHealthInfoCommand)
            .Times(static_cast<int>(testTypes.size()))
            .WillRepeatedly(testing::Invoke([](RAIIHandle &handle,
                                         std::shared_ptr<StorageNVMeGetLogPageSmartHealthInfoCommand> nvmeSmartCommand) {
                std::shared_ptr<StorageNVMeCommandResult> nvmeCommandResult = nvmeSmartCommand->GetStorageNVMeCommandResult();
                nvmeCommandResult->SetStatusType(NVMeStatusCodeType::GenericCommandStatus);
                nvmeCommandResult->SetStatusCode(NVMeGenericCommandStatusCode::NamespaceNotReady);
            }));

    // Execute Diagnostic Provider for all TestTypes
    for (const auto &testType: testTypes) {
        NVMeCheckSmartDiagnosticProvider smartDiagnosisProvider(testType);
        ASSERT_TRUE(smartDiagnosisProvider.Supports(mockNVMeDevice));

        // Validate Results
        std::vector<std::shared_ptr<DiagnosticLogBase>> diagnosticResult = smartDiagnosisProvider.Diagnose(mockNVMeDevice);
        ASSERT_EQ(diagnosticResult.size(), 1);
        ASSERT_EQ(diagnosticResult[0]->GetStorageTestLogType(), StorageTestLogType::Diagnosis);

        const std::shared_ptr<DiagnosticResultLog> diagnosticResultLog = std::dynamic_pointer_cast<DiagnosticResultLog>(diagnosticResult[0]);
        ASSERT_NE(diagnosticResultLog, nullptr);
        ASSERT_EQ(diagnosticResultLog->GetDiagnosisResultType(), DiagnosisResultType::Fail);
        ASSERT_EQ(diagnosticResultLog->GetVerdict(), "nvme-get-log-page-smart-health-info-command-fail");
        ASSERT_EQ(diagnosticResultLog->GetMessage(), "Get Log Page(LID = 2) - Status Type(0),Status Code(Namespace Not Ready)");
    }
}

TEST(NVMeCheckSmartDiagnosisProviderTest, ValidateMeasurementsForInitialAndFieldTest) {

    const unsigned char NVME_SMART_TEST_CRITICAL_WARNING_VALUE = 0x18;
    const unsigned char NVME_SMART_TEST_AVAILABLE_SPARE_VALUE = 0x9C;
    const unsigned char NVME_SMART_TEST_AVAILABLE_SPARE_THRESHOLD_VALUE = 10;
    const unsigned char NVME_SMART_TEST_PERCENTAGE_USED_VALUE = 0xAB;

    std::vector<TestType> testTypes = {TestType::Initial, TestType::Field};

    // Create Mock Device
    std::shared_ptr<NVMeStorageInfo> mockNVMeDevice = StorageFixture::CreateMockNVMeDevice();

    // Mock Storage Controller
    std::shared_ptr<MockNVMeStorageController> storageController
        = std::dynamic_pointer_cast<MockNVMeStorageController>(mockNVMeDevice->GetStorageController());
    EXPECT_CALL(*storageController, SendGetLogPageSmartHealthInfoCommand)
            .Times(static_cast<int>(testTypes.size()))
            .WillRepeatedly(testing::Invoke([](RAIIHandle &handle,
                                         std::shared_ptr<StorageNVMeGetLogPageSmartHealthInfoCommand> nvmeSmartCommand) {
                std::shared_ptr<StorageNVMeCommandResult> nvmeCommandResult = nvmeSmartCommand->GetStorageNVMeCommandResult();
                std::vector<unsigned char> &dataBufferRef = nvmeSmartCommand->GetDataBufferRef();
                if (dataBufferRef.size() != 4096)
                    dataBufferRef.assign(4096, 0);

                // Capacitor Fail = Bytes 0(Critical Warning), Bit 4
                // Read Only Mode = Bytes 0(Critical Warning), Bit 3
                dataBufferRef[0] = NVME_SMART_TEST_CRITICAL_WARNING_VALUE;

                // Available Spare = Bytes 3
                dataBufferRef[3] = NVME_SMART_TEST_AVAILABLE_SPARE_VALUE;

                // Available Spare Threshold = Bytes 4
                dataBufferRef[4] = NVME_SMART_TEST_AVAILABLE_SPARE_THRESHOLD_VALUE;

                // Percentage Used = Bytes 5
                dataBufferRef[5] = NVME_SMART_TEST_PERCENTAGE_USED_VALUE;

                nvmeCommandResult->SetStatusType(NVMeStatusCodeType::GenericCommandStatus);
                nvmeCommandResult->SetStatusCode(NVMeGenericCommandStatusCode::SuccessfulCompletion);
            }));

    // Execute Diagnostic Provider for all TestTypes
    for (const auto& testType: testTypes) {
        NVMeCheckSmartDiagnosticProvider smartDiagnosisProvider(testType);
        ASSERT_TRUE(smartDiagnosisProvider.Supports(mockNVMeDevice));
        std::vector<std::shared_ptr<DiagnosticLogBase>> diagnosticResult = smartDiagnosisProvider.Diagnose(mockNVMeDevice);

        // Validate Results
        ASSERT_EQ(diagnosticResult.size(), 5);

        // 1) Diagnosis for NVMe Get Log Page Command
        int currentDiagnosticResultIndex = 0;
        {
            const std::shared_ptr<DiagnosticResultLog> diagnosticResultLog = std::dynamic_pointer_cast<DiagnosticResultLog>(diagnosticResult[currentDiagnosticResultIndex]);
            ASSERT_EQ(diagnosticResultLog->GetStorageTestLogType(), StorageTestLogType::Diagnosis);
            ASSERT_NE(diagnosticResultLog, nullptr);
            ASSERT_EQ(diagnosticResultLog->GetDiagnosisResultType(), DiagnosisResultType::Pass);
            ASSERT_EQ(diagnosticResultLog->GetVerdict(), "nvme-get-log-page-smart-health-info-command-pass");
            ASSERT_EQ(diagnosticResultLog->GetMessage(), "NVMe Get Log Page command has been sent successfully.");
        }

        // 2) Measurement for Bytes 0(Critical Warning) / Bit 3(Capacitor Fail) in NVMe SMART data
        currentDiagnosticResultIndex++;
        {
            const std::shared_ptr<DiagnosticMeasurementLog> diagnosticResultLog = std::dynamic_pointer_cast<DiagnosticMeasurementLog>(diagnosticResult[currentDiagnosticResultIndex]);
            ASSERT_EQ(diagnosticResultLog->GetStorageTestLogType(), StorageTestLogType::Measurement);
            ASSERT_NE(diagnosticResultLog, nullptr);
            ASSERT_EQ(diagnosticResultLog->GetName(), "check-capacitor-status");
            ASSERT_EQ(diagnosticResultLog->GetUnit(), "Bit");
            ASSERT_EQ(diagnosticResultLog->GetActualValue(), ocpdiag::results::Variant{true});
            ASSERT_TRUE(diagnosticResultLog->GetMetadataJson().empty());

            std::vector<StorageTestMeasurementValidator> validators = diagnosticResultLog->GetValidators();
            ASSERT_EQ(validators.size(), 1);
            ASSERT_EQ(validators[0].GetStorageTestValidatorType(), StorageTestValidatorType::NotEqual);
            ASSERT_EQ(validators[0].GetValidatorName(), "nvme_smart_critical_warning_bit_4");
            ASSERT_EQ(validators[0].GetValue(), std::vector<ocpdiag::results::Variant>{ false } );
        }

        // 3) Measurement for Bytes 0(Critical Warning) / Bit 4(Read Only Mode) in NVMe SMART data
        currentDiagnosticResultIndex++;
        {
            const std::shared_ptr<DiagnosticMeasurementLog> diagnosticResultLog = std::dynamic_pointer_cast<DiagnosticMeasurementLog>(diagnosticResult[currentDiagnosticResultIndex]);
            ASSERT_EQ(diagnosticResultLog->GetStorageTestLogType(), StorageTestLogType::Measurement);
            ASSERT_NE(diagnosticResultLog, nullptr);
            ASSERT_EQ(diagnosticResultLog->GetName(), "check-read-only-mode");
            ASSERT_EQ(diagnosticResultLog->GetUnit(), "Bit");
            ASSERT_EQ(diagnosticResultLog->GetActualValue(), ocpdiag::results::Variant{true});
            ASSERT_TRUE(diagnosticResultLog->GetMetadataJson().empty());

            std::vector<StorageTestMeasurementValidator> validators = diagnosticResultLog->GetValidators();
            ASSERT_EQ(validators.size(), 1);
            ASSERT_EQ(validators[0].GetStorageTestValidatorType(), StorageTestValidatorType::NotEqual);
            ASSERT_EQ(validators[0].GetValidatorName(), "nvme_smart_critical_warning_bit_3");
            ASSERT_EQ(validators[0].GetValue(), std::vector<ocpdiag::results::Variant>{ false } );
        }

        // 4) Measurement for Bytes 3(Available Spare) in NVMe SMART data
        currentDiagnosticResultIndex++;
        {
            const std::shared_ptr<DiagnosticMeasurementLog> diagnosticResultLog = std::dynamic_pointer_cast<DiagnosticMeasurementLog>(diagnosticResult[currentDiagnosticResultIndex]);
            ASSERT_EQ(diagnosticResultLog->GetStorageTestLogType(), StorageTestLogType::Measurement);
            ASSERT_NE(diagnosticResultLog, nullptr);
            ASSERT_EQ(diagnosticResultLog->GetName(), "check-available-spare");
            ASSERT_EQ(diagnosticResultLog->GetUnit(), "Percentage");
            ASSERT_EQ(diagnosticResultLog->GetActualValue(), ocpdiag::results::Variant{ static_cast<double>(NVME_SMART_TEST_AVAILABLE_SPARE_VALUE) });
            ASSERT_TRUE(diagnosticResultLog->GetMetadataJson().empty());

            std::vector<StorageTestMeasurementValidator> validators = diagnosticResultLog->GetValidators();
            ASSERT_EQ(validators.size(), 1);

            if(testType == TestType::Initial)
            {
                ASSERT_EQ(validators[0].GetStorageTestValidatorType(), StorageTestValidatorType::Equal);
                ASSERT_EQ(validators[0].GetValidatorName(), "nvme_smart_available_spare_100_percent");
                ASSERT_EQ(validators[0].GetValue(), std::vector<ocpdiag::results::Variant>{ 100.0 } );
            }
            else if(testType == TestType::Field)
            {
                ASSERT_EQ(validators[0].GetStorageTestValidatorType(), StorageTestValidatorType::GreaterThanOrEqual);
                ASSERT_EQ(validators[0].GetValidatorName(), "nvme_smart_available_spare_threshold");
                ASSERT_EQ(validators[0].GetValue(), std::vector<ocpdiag::results::Variant>{
                    static_cast<double>(NVME_SMART_TEST_AVAILABLE_SPARE_THRESHOLD_VALUE) } );
            }
            else
            {
                assert(false && "TestType is invalid.");
            }

        }

        // 5) Measurement for Bytes 5(Percentage Used) in NVMe SMART data
        currentDiagnosticResultIndex++;
        {
            const std::shared_ptr<DiagnosticMeasurementLog> diagnosticResultLog = std::dynamic_pointer_cast<DiagnosticMeasurementLog>(diagnosticResult[currentDiagnosticResultIndex]);
            ASSERT_EQ(diagnosticResultLog->GetStorageTestLogType(), StorageTestLogType::Measurement);
            ASSERT_NE(diagnosticResultLog, nullptr);
            ASSERT_EQ(diagnosticResultLog->GetName(), "check-percentage-used");
            ASSERT_EQ(diagnosticResultLog->GetUnit(), "Percentage");
            ASSERT_EQ(diagnosticResultLog->GetActualValue(), ocpdiag::results::Variant{ static_cast<double>(NVME_SMART_TEST_PERCENTAGE_USED_VALUE) });
            ASSERT_TRUE(diagnosticResultLog->GetMetadataJson().empty());

            std::vector<StorageTestMeasurementValidator> validators = diagnosticResultLog->GetValidators();
            ASSERT_EQ(validators.size(), 1);
            ASSERT_EQ(validators[0].GetStorageTestValidatorType(), StorageTestValidatorType::LessThan);
            ASSERT_EQ(validators[0].GetValidatorName(), "nvme_smart_percentage_used_100_percent");
            ASSERT_EQ(validators[0].GetValue(), std::vector<ocpdiag::results::Variant>{ 100.0 } );
        }
    }


}

}