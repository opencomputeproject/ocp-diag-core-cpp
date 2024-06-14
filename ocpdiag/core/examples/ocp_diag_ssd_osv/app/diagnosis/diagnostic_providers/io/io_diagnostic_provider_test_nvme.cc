// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "gtest/gtest.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_command/nvme/admin/storage_nvme_command_identify_controller.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_scanner/storage_fixture.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_controller/mock_nvme_storage_controller.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/io/io_diagnostic_provider.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_error_log.h"

namespace ocp_diag_ssd_osv {

struct IoTestAreaTestParameters
{
    enum : unsigned long long
    {
        BLOCK_COUNT_4KB = (1ull * 1024 * 1024 * 1024) / 512,
        BLOCK_COUNT_1GB = (1ull * 1024 * 1024 * 1024) / 512,
        BLOCK_COUNT_512GB = BLOCK_COUNT_1GB * 512,
    };

    unsigned long long startLBA = 0;
    unsigned long long endLBAPlusOne = 0;
    unsigned long long deviceMaxLBA = 0;
    IoCmdRequestSize ioCmdRequestSize = IoCmdRequestSize::IO_SIZE_4KB;

    IoTestAreaTestParameters() = default;
    IoTestAreaTestParameters(unsigned long long int startLba,
                             unsigned long long int endLbaPlusOne,
                             unsigned long long int deviceMaxLba,
                             IoCmdRequestSize ioCmdRequestSize) :
            startLBA(startLba), endLBAPlusOne(endLbaPlusOne), deviceMaxLBA(deviceMaxLba), ioCmdRequestSize(ioCmdRequestSize) {}
};

class NVMeIoDiagnosticProviderTestHelper
{
public:
    static std::vector<std::shared_ptr<DiagnosticLogBase>> CreateIoAreaTest(const IoTestAreaTestParameters& parameters) {

        // Create Mock Device
        std::shared_ptr<NVMeStorageInfo> mockNVMeDevice = StorageFixture::CreateMockNVMeDevice({"/dev/nvme0"});

        // Mock Storage Controller - Density of the mock device will be 1GB by IdentifyNamespaceCommand.
        std::shared_ptr<MockNVMeStorageController> storageController
                = std::dynamic_pointer_cast<MockNVMeStorageController>(mockNVMeDevice->GetStorageController());
        EXPECT_CALL(*storageController, SendIdentifyNamespaceCommand)
                .Times(1)
                .WillOnce(testing::Invoke([&parameters = parameters](RAIIHandle &handle,
                                             std::shared_ptr<StorageNVMeIdentifyNamespaceCommand> nvmeIdentifyNamespaceCommand) {
                    std::shared_ptr<StorageNVMeCommandResult> nvmeCommandResult = nvmeIdentifyNamespaceCommand->GetStorageNVMeCommandResult();
                    std::vector<unsigned char>& dataBufferRef = nvmeIdentifyNamespaceCommand->GetDataBufferRef();
                    if(dataBufferRef.size() != 4096)
                        dataBufferRef.assign(4096, 0);

                    // NVMe Capacity = 1GB
                    BufferUtils::SetNumberToByteBufferLittleEndian(dataBufferRef, 8, 8, parameters.deviceMaxLBA);

                    // LBA Format Namespace Supports : lbaDataSize(PowerOfTwo) = 2^9
                    dataBufferRef[130] = 9;

                    // SuccessfulCompletion
                    nvmeCommandResult->SetStatusType(NVMeStatusCodeType::GenericCommandStatus);
                    nvmeCommandResult->SetStatusCode(NVMeGenericCommandStatusCode::SuccessfulCompletion);
                }));

        // Execute Diagnostic Provider
        IoDiagnosticProvider ioDiagnosticProvider(
                IoTestOption::CreateTimeBasedRandomIoTestOption(
                        parameters.startLBA,
                        parameters.endLBAPlusOne,
                        parameters.ioCmdRequestSize,
                        (10) /* 10 seconds */,
                        false,
                        1));

        return ioDiagnosticProvider.Diagnose(mockNVMeDevice);
    }

    static void ValidateIdentifyNamespaceCommandResultIsSuccessful(const std::shared_ptr<DiagnosticResultLog>& diagnosticResultLog) {
        ASSERT_EQ(diagnosticResultLog->GetStorageTestLogType(), StorageTestLogType::Diagnosis);
        ASSERT_NE(diagnosticResultLog, nullptr);
        ASSERT_EQ(diagnosticResultLog->GetDiagnosisResultType(), DiagnosisResultType::Pass);
        ASSERT_EQ(diagnosticResultLog->GetVerdict(), "nvme-identify-namespace-command-pass");
        ASSERT_EQ(diagnosticResultLog->GetMessage(), "NVMe Identify Namespace command has been sent successfully.");
    }
};

struct NVMeIoDiagnosticProviderTest : public testing::TestWithParam<IoTestAreaTestParameters> {};

INSTANTIATE_TEST_SUITE_P(IoDiagnosticProviderTestForTestAreaExceedsDeviceCapacityCases,
                         NVMeIoDiagnosticProviderTest,
                         ::testing::Values(
                                 IoTestAreaTestParameters{IoTestAreaTestParameters::BLOCK_COUNT_1GB,
                                                          IoTestAreaTestParameters::BLOCK_COUNT_1GB,
                                                          IoTestAreaTestParameters::BLOCK_COUNT_1GB,
                                                          IoCmdRequestSize::IO_SIZE_4KB,},
                                 IoTestAreaTestParameters{IoTestAreaTestParameters::BLOCK_COUNT_1GB,
                                                          1,
                                                          IoTestAreaTestParameters::BLOCK_COUNT_512GB,
                                                          IoCmdRequestSize::IO_SIZE_128KB}
                         ));

TEST_P(NVMeIoDiagnosticProviderTest, TestAreaExceedsDeviceCapacityCase)
{
    IoTestAreaTestParameters ioTestAreaTestParameters = NVMeIoDiagnosticProviderTest::GetParam();

    std::vector<std::shared_ptr<DiagnosticLogBase>> diagnosticResult = NVMeIoDiagnosticProviderTestHelper::CreateIoAreaTest(ioTestAreaTestParameters);

    // Validate Results
    ASSERT_EQ(diagnosticResult.size(), 2);

    int currentDiagnosticResultIndex = 0;
    {
        const std::shared_ptr<DiagnosticResultLog> diagnosticResultLog = std::dynamic_pointer_cast<DiagnosticResultLog>(diagnosticResult[currentDiagnosticResultIndex]);
        NVMeIoDiagnosticProviderTestHelper::ValidateIdentifyNamespaceCommandResultIsSuccessful(diagnosticResultLog);
    }

    currentDiagnosticResultIndex++;
    {
        const std::shared_ptr<DiagnosticErrorLog> diagnosticErrorLog = std::dynamic_pointer_cast<DiagnosticErrorLog>(diagnosticResult[currentDiagnosticResultIndex]);
        ASSERT_NE(diagnosticErrorLog, nullptr);
        ASSERT_EQ(diagnosticErrorLog->GetStorageTestLogType(), StorageTestLogType::Error);
        ASSERT_EQ(diagnosticErrorLog->GetSymptom(), "Parameter Error");

        // It doesn't check Message of DiagnosticErrorLog.
    }

}

TEST(NVMeIoDiagnosticProviderTest, DeviceMaxLbaIsZeroCase)
{
    IoTestAreaTestParameters ioTestAreaTestParameters;
    ioTestAreaTestParameters.startLBA = IoTestAreaTestParameters::BLOCK_COUNT_1GB;
    ioTestAreaTestParameters.endLBAPlusOne = IoTestAreaTestParameters::BLOCK_COUNT_1GB;
    ioTestAreaTestParameters.ioCmdRequestSize = IoCmdRequestSize::IO_SIZE_4KB;
    ioTestAreaTestParameters.deviceMaxLBA = 0;

    std::vector<std::shared_ptr<DiagnosticLogBase>> diagnosticResult = NVMeIoDiagnosticProviderTestHelper::CreateIoAreaTest(ioTestAreaTestParameters);

    // Validate Results
    ASSERT_EQ(diagnosticResult.size(), 2);

    int currentDiagnosticResultIndex = 0;
    {
        const std::shared_ptr<DiagnosticResultLog> diagnosticResultLog = std::dynamic_pointer_cast<DiagnosticResultLog>(diagnosticResult[currentDiagnosticResultIndex]);
        NVMeIoDiagnosticProviderTestHelper::ValidateIdentifyNamespaceCommandResultIsSuccessful(diagnosticResultLog);
    }

    currentDiagnosticResultIndex++;
    {
        const std::shared_ptr<DiagnosticErrorLog> diagnosticErrorLog = std::dynamic_pointer_cast<DiagnosticErrorLog>(diagnosticResult[currentDiagnosticResultIndex]);
        ASSERT_NE(diagnosticErrorLog, nullptr);
        ASSERT_EQ(diagnosticErrorLog->GetStorageTestLogType(), StorageTestLogType::Error);
        ASSERT_EQ(diagnosticErrorLog->GetSymptom(), "Parameter Error");
        ASSERT_EQ(diagnosticErrorLog->GetMessage(), "deviceMaxLBA_ must not be 0.");
    }

}

TEST(NVMeIoDiagnosticProviderTest, NVMeIdentifyNamespaceCommandFailCase)
{
    // Create Mock Device
    std::shared_ptr<NVMeStorageInfo> mockNVMeDevice = StorageFixture::CreateMockNVMeDevice({"/dev/nvme0"});

    // Mock Storage Controller
    std::shared_ptr<MockNVMeStorageController> storageController
            = std::dynamic_pointer_cast<MockNVMeStorageController>(mockNVMeDevice->GetStorageController());
    EXPECT_CALL(*storageController, SendIdentifyNamespaceCommand)
            .Times(1)
            .WillOnce(testing::Invoke([](RAIIHandle &handle,
                                         std::shared_ptr<StorageNVMeIdentifyNamespaceCommand> nvmeIdentifyNamespaceCommand) {
                std::shared_ptr<StorageNVMeCommandResult> nvmeCommandResult = nvmeIdentifyNamespaceCommand->GetStorageNVMeCommandResult();
                std::vector<unsigned char>& dataBufferRef = nvmeIdentifyNamespaceCommand->GetDataBufferRef();

                if(dataBufferRef.size() != 4096)
                    dataBufferRef.assign(4096, 0);

                // Error
                nvmeCommandResult->SetStatusType(NVMeStatusCodeType::GenericCommandStatus);
                nvmeCommandResult->SetStatusCode(NVMeGenericCommandStatusCode::InternalError);
            }));

    // Execute Diagnostic Provider
    IoDiagnosticProvider ioDiagnosticProvider(
            IoTestOption::CreateTimeBasedRandomIoTestOption(
                    0,
                    IoTestAreaTestParameters::BLOCK_COUNT_1GB,
                    IoCmdRequestSize::IO_SIZE_4KB,
                    (10) /* 10 seconds */,
                    false,
                    1));

    std::vector<std::shared_ptr<DiagnosticLogBase>> diagnosticResult = ioDiagnosticProvider.Diagnose(mockNVMeDevice);

    // Validate Results
    ASSERT_EQ(diagnosticResult.size(), 1);

    int currentDiagnosticResultIndex = 0;
    {
        const std::shared_ptr<DiagnosticResultLog> diagnosticResultLog = std::dynamic_pointer_cast<DiagnosticResultLog>(diagnosticResult[currentDiagnosticResultIndex]);
        ASSERT_NE(diagnosticResultLog, nullptr);
        ASSERT_EQ(diagnosticResultLog->GetStorageTestLogType(), StorageTestLogType::Diagnosis);
        ASSERT_EQ(diagnosticResultLog->GetDiagnosisResultType(), DiagnosisResultType::Fail);
        ASSERT_EQ(diagnosticResultLog->GetVerdict(), "nvme-identify-namespace-command-fail");
        ASSERT_EQ(diagnosticResultLog->GetMessage(), "NVMe Identify Namespace #1 - Status Type(0),Status Code(Internal Error)");
    }

}

TEST(NVMeIoDiagnosticProviderTest, NVMeNotActiveNamespaceCase)
{
    // Create Mock Device
    std::shared_ptr<NVMeStorageInfo> mockNVMeDevice = StorageFixture::CreateMockNVMeDevice({"/dev/nvme0"});

    // Mock Storage Controller
    std::shared_ptr<MockNVMeStorageController> storageController
            = std::dynamic_pointer_cast<MockNVMeStorageController>(mockNVMeDevice->GetStorageController());
    EXPECT_CALL(*storageController, SendIdentifyNamespaceCommand)
            .Times(1)
            .WillOnce(testing::Invoke([](RAIIHandle &handle,
                                                                 std::shared_ptr<StorageNVMeIdentifyNamespaceCommand> nvmeIdentifyNamespaceCommand) {
                std::shared_ptr<StorageNVMeCommandResult> nvmeCommandResult = nvmeIdentifyNamespaceCommand->GetStorageNVMeCommandResult();
                std::vector<unsigned char>& dataBufferRef = nvmeIdentifyNamespaceCommand->GetDataBufferRef();

                // Returns all zero data. According to NVMe spec, if the specified namespace is not an active NSID,
                // then the controller returns a zero filled data structure.
                if(dataBufferRef.size() != 4096)
                    dataBufferRef.assign(4096, 0);

                // SuccessfulCompletion
                nvmeCommandResult->SetStatusType(NVMeStatusCodeType::GenericCommandStatus);
                nvmeCommandResult->SetStatusCode(NVMeGenericCommandStatusCode::SuccessfulCompletion);
            }));

    // Execute Diagnostic Provider
    IoDiagnosticProvider ioDiagnosticProvider(
            IoTestOption::CreateTimeBasedRandomIoTestOption(
                    0,
                    IoTestAreaTestParameters::BLOCK_COUNT_1GB,
                    IoCmdRequestSize::IO_SIZE_4KB,
                    (10) /* 10 seconds */,
                    false,
                    1));

    std::vector<std::shared_ptr<DiagnosticLogBase>> diagnosticResult = ioDiagnosticProvider.Diagnose(mockNVMeDevice);

    // Validate Results
    ASSERT_EQ(diagnosticResult.size(), 2);

    int currentDiagnosticResultIndex = 0;
    {
        const std::shared_ptr<DiagnosticResultLog> diagnosticResultLog = std::dynamic_pointer_cast<DiagnosticResultLog>(diagnosticResult[currentDiagnosticResultIndex]);
        NVMeIoDiagnosticProviderTestHelper::ValidateIdentifyNamespaceCommandResultIsSuccessful(diagnosticResultLog);
    }

    currentDiagnosticResultIndex++;
    {
        const std::shared_ptr<DiagnosticErrorLog> diagnosticErrorLog = std::dynamic_pointer_cast<DiagnosticErrorLog>(diagnosticResult[currentDiagnosticResultIndex]);
        ASSERT_NE(diagnosticErrorLog, nullptr);
        ASSERT_EQ(diagnosticErrorLog->GetStorageTestLogType(), StorageTestLogType::Error);
        ASSERT_EQ(diagnosticErrorLog->GetSymptom(), "Not Active NVMe Namespace");
        ASSERT_EQ(diagnosticErrorLog->GetMessage(), "Given namespace #1 is not active.");
    }
    
}

TEST(NVMeIoDiagnosticProviderTest, MetadataWithNonExtendedLBAIsNotSupportedInLinuxInboxDriver)
{
    // Create Mock Device.
    std::shared_ptr<NVMeStorageInfo> mockNVMeDevice = StorageFixture::CreateMockNVMeDevice(
            "/dev/nvme0",
            StorageControllerType::LinuxInbox
    );

    // Mock Storage Controller
    std::shared_ptr<MockNVMeStorageController> storageController
            = std::dynamic_pointer_cast<MockNVMeStorageController>(mockNVMeDevice->GetStorageController());
    EXPECT_CALL(*storageController, SendIdentifyNamespaceCommand)
            .Times(1)
            .WillOnce(testing::Invoke([](RAIIHandle &handle,
                                         std::shared_ptr<StorageNVMeIdentifyNamespaceCommand> nvmeIdentifyNamespaceCommand) {
                std::shared_ptr<StorageNVMeCommandResult> nvmeCommandResult = nvmeIdentifyNamespaceCommand->GetStorageNVMeCommandResult();
                std::vector<unsigned char>& dataBufferRef = nvmeIdentifyNamespaceCommand->GetDataBufferRef();

                if(dataBufferRef.size() != 4096)
                    dataBufferRef.assign(4096, 0);

                // Non-extended LBA = Bytes 26 Formatted LBA Size (FLBAS) Bit 4 is 0
                // NVMe Capacity = 1GB
                BufferUtils::SetNumberToByteBufferLittleEndian(dataBufferRef, 8, 8, IoTestAreaTestParameters::BLOCK_COUNT_512GB);

                // LBA Format Namespace Supports : lbaDataSize(PowerOfTwo) = 2^9
                dataBufferRef[130] = 9;

                // Metasize = 64
                dataBufferRef[128] = 64;

                // SuccessfulCompletion
                nvmeCommandResult->SetStatusType(NVMeStatusCodeType::GenericCommandStatus);
                nvmeCommandResult->SetStatusCode(NVMeGenericCommandStatusCode::SuccessfulCompletion);
            }));

    // Execute Diagnostic Provider
    IoDiagnosticProvider ioDiagnosticProvider(
            IoTestOption::CreateTimeBasedRandomIoTestOption(
                    0,
                    IoTestAreaTestParameters::BLOCK_COUNT_1GB,
                    IoCmdRequestSize::IO_SIZE_4KB,
                    (10) /* 10 seconds */,
                    false,
                    1));

    std::vector<std::shared_ptr<DiagnosticLogBase>> diagnosticResult = ioDiagnosticProvider.Diagnose(mockNVMeDevice);

    // Validate Results
    ASSERT_EQ(diagnosticResult.size(), 2);

    int currentDiagnosticResultIndex = 0;
    {
        const std::shared_ptr<DiagnosticResultLog> diagnosticResultLog = std::dynamic_pointer_cast<DiagnosticResultLog>(diagnosticResult[currentDiagnosticResultIndex]);
        NVMeIoDiagnosticProviderTestHelper::ValidateIdentifyNamespaceCommandResultIsSuccessful(diagnosticResultLog);
    }

    currentDiagnosticResultIndex++;
    {
        const std::shared_ptr<DiagnosticErrorLog> diagnosticErrorLog = std::dynamic_pointer_cast<DiagnosticErrorLog>(diagnosticResult[currentDiagnosticResultIndex]);
        ASSERT_NE(diagnosticErrorLog, nullptr);
        ASSERT_EQ(diagnosticErrorLog->GetStorageTestLogType(), StorageTestLogType::Error);
        ASSERT_EQ(diagnosticErrorLog->GetSymptom(), "Metadata is not extended LBA");
        ASSERT_EQ(diagnosticErrorLog->GetMessage(), "Metadata with not Extended LBA is not supported in Linux Inbox Driver.");
    }
}

}

