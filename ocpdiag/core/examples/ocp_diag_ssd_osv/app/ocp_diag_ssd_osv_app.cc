// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/ocp_diag_ssd_osv_app.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_scanner/storage_scan_manager.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/command_parser_manager.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/command_parser/target_disks_command_parser.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/command_parser/test_type_command_parser.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_manager.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/identify/nvme/nvme_check_identify_diagnostic_provider.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/io/io_diagnostic_provider.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_providers/smart/nvme/nvme_check_smart_diagnostic_provider.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/storage/storage_scanner/nvme/nvme_storage_scanner_linux_inbox.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/command/bad_argument_exception.h"

namespace ocp_diag_ssd_osv {

OcpDiagSsdApp::OcpDiagSsdApp(std::unique_ptr<ocpdiag::results::internal::ArtifactWriter> writer)
        : run_(
        {
                .name = "ocp_diag_ssd_osv",
                .version = "1.0",
                .command_line = "ocp_diag_ssd_osv", // NOTE : command_line is actually not used for Samsung SSD OSV Test.
        }, std::move(writer)) {
}

void OcpDiagSsdApp::ExecuteTest(const std::vector<char*>& commandLine) {

    try
    {
        if(!UpdateOcpDiagSsdTestProperties(commandLine)) return;
        if(!CheckIsRoot()) return;
        if(!CreateDutInfo()) return;

        RunDiagnostic();
    }
    catch(std::exception& exception)
    {
        GetOcpDiagTestRunRef().AddPreStartError({
                                                        .symptom = "Exception occurred",
                                                        .message = exception.what(),
                                                });
    }
    catch(...) // Unknown Error
    {
        GetOcpDiagTestRunRef().AddPreStartError({
                                            .symptom = "Unknown Exception occurred",
                                            .message = "Unknown Error occurred.",
                                    });
    }

}

bool OcpDiagSsdApp::CheckIsRoot() {
    if(getuid() > 0) // not root
    {
        GetOcpDiagTestRunRef().AddPreStartError({
                                            .symptom = "Permission Denied",
                                            .message = "Please execute this application with root privileges.",
                                    });
        return false;
    }
    return true;
}

bool OcpDiagSsdApp::UpdateOcpDiagSsdTestProperties(const std::vector<char *> &commandLine) {

    try
    {
        ocpDiagSsdTestProperties_ = CommandParserManager({
                                                                 std::make_shared<TargetDisksCommandParser>(),
                                                                 std::make_shared<TestTypeCommandParser>(),
                                                         }).ParseProperties(commandLine);

        // Default Test Type = Initial(Incoming) Test
        if(ocpDiagSsdTestProperties_.testType == TestType::Unknown) {
            ocpDiagSsdTestProperties_.testType = TestType::Initial;

            GetOcpDiagTestRunRef().AddPreStartLog({
                .severity = ocpdiag::results::LogSeverity::kWarning,
                .message = "TestType is [Initial] by default. Explicitly configure TestType parameter to disable this warning.",
            });
        }

        return true;
    }
    catch(BadArgumentException& exception)
    {
        GetOcpDiagTestRunRef().AddPreStartError({
                                            .symptom = "Bad Arguments",
                                            .message = exception.what(),
                                    });
        return false;
    }

}


bool OcpDiagSsdApp::CreateDutInfo() {

    std::unique_ptr<DutInfo> dutInfo = std::make_unique<DutInfo>("dut_ssd", "1");
    std::vector<std::shared_ptr<StorageInfoBase>> scannedStorages = ScanStorages();

    std::function<bool(std::string)> targetDiskFilter = [](const std::string& devPath){ return true; };
    if(!ocpDiagSsdTestProperties_.targetDiskPaths.empty()) {
        targetDiskFilter = [&targetDiskPaths = ocpDiagSsdTestProperties_.targetDiskPaths](const std::string& devPath) {
            return std::find(targetDiskPaths.begin(),
                             targetDiskPaths.end(),
                             devPath) != targetDiskPaths.end();
        };
    }

    targetStorageDevices_.clear();
    for (auto& scannedStorage: scannedStorages) {
        if(targetDiskFilter(scannedStorage->GetDevicePath()))
        {
            scannedStorage->SetRegisteredHardwareInfo(dutInfo->AddHardwareInfo(scannedStorage->To()));
            targetStorageDevices_.push_back(scannedStorage);
        }
    }

    // No Target Disk
    if(targetStorageDevices_.empty()) {
        GetOcpDiagTestRunRef().AddPreStartError({
                                            .symptom = "No Target Disk",
                                            .message = "There is no supported disk.",
                                    });
        return false;
    }

    run_.StartAndRegisterDutInfo(std::move(dutInfo));
    return true;

}

std::vector<std::shared_ptr<StorageInfoBase>> OcpDiagSsdApp::ScanStorages() {

    std::vector<std::shared_ptr<StorageInfoBase>> ret;
    StorageScanManager storageScanManager {{std::make_shared<NVMeStorageScannerLinuxInbox>()}};

    // NVMe
    {
        std::vector<std::shared_ptr<NVMeStorageInfo>> nvmeStorageDevices = storageScanManager.GetAllRecognizedNVMeStorageDevices();
        for (auto& storageDevice: nvmeStorageDevices) {
            ret.push_back(storageDevice);
        }
    }

    return ret;

}

void OcpDiagSsdApp::RunDiagnostic() {
    // Test Scenario
    DiagnosticManager diagnosisManager(GetOcpDiagTestRunRef(), {
            // 1. Check NVMe Identify
            std::make_shared<NVMeCheckIdentifyDiagnosticProvider>(),
            // 2. I/O Test
            std::make_shared<IoDiagnosticProvider>(
                    IoTestOption::CreateTimeBasedRandomIoTestOption( /* Random I/O Test */
                            0, /* Start LBA = 0 */
                            0 /* End LBA. 0 means Device Full LBA */,
                            IoCmdRequestSize::IO_SIZE_4KB, /* Read Command Block Size = 4KB */
                            (5 * 60) /* Test Time = 5 mins */,
                            false,  /* Is Write Test = false */
                            1)), /* Target NVMe Namespace = 1 */
            // 3. Check NVMe SMART Health Info
            std::make_shared<NVMeCheckSmartDiagnosticProvider>(ocpDiagSsdTestProperties_.testType),
    });

    diagnosisManager.DoDiagnose(targetStorageDevices_);
}

}


