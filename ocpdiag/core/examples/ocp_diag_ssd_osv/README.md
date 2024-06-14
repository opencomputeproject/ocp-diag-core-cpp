# ocp-diag-ssd-osv

## Overview
**ocp-diag-ssd-osv** is a open-source SSD **OSV(On-Site Verification)** software that provides simple OCP-compliant on-site diagnosis.
Two modes of testing are available : Initial and Field.
* Initial Test : These test helps to identify problems with the SSD during the manufacturing process or before the end-user uses it for the first time.
* Field Test : These test helps to identify problems with the SSD being used by the end-user.

### Supported Protocol
It currently supports NVMe only.

### Test Sequence
1. Send Identify Controller command.
2. Execute 4K Full Random Read Test (5 mins)
- For NVMe product, Identify Namespace command for namespace #1 will be sent before the test in order to get LBA Format status. if there is no namespace #1, This test will be skipped.
3. Check NVMe SMART / Health Information
- Check if Bytes 0 (Critical Warning) Bit 4 is 0. (Capacitor Fail)
- Check if Bytes 0 (Critical Warning) Bit 3 is 0. (Read Only Mode)
- (If Test Type is **Initial**) Check if Bytes 3 (Available Spare) is 100.
- (If Test Type is **Field**) Check if Bytes 3 (Available Spare) is greater than or equal to Bytes 4 (Available Spare Threshold).
- Check If Bytes 5 (Percentage Used) is less than 100.

## Build the ocp-diag-ssd-osv target
The following command line builds the **ocp-diag-ssd-osv** executable.

```shell
<your_terminal>$ > sudo bazel build //ocpdiag/core/examples/ocp_diag_ssd_osv:ocp_diag_ssd_osv --cxxopt='-std=c++17'
```
The executable appears under `./bazel-bin/ocpdiag/core/examples/ocp-diag-ssd-osv/ocp-diag-ssd-osv` as indicated by bazel logs.

## Running the ocp-diag-ssd-osv

### Options
* -i, --initial : Execute test as initial Test Mode. (default)
* -f, --field : Execute test as field Test Mode.
* -d, --disk : Target disks. If you don't specify this option, Target disks will be all disks detected in your system. You can use this option multiple times to specify multiple target disks. ex) -d /dev/nvme1 -d /dev/nvme2

### Example
```shell
# running the built binary. test type = field(-f), target disks = /dev/nvme0, /dev/nvme1
<your_terminal>$ > sudo ./bazel-bin/ocpdiag/core/examples/ocp_diag_ssd_osv/ocp_diag_ssd_osv -f -d /dev/nvme0 -d /dev/nvme1
{"schemaVersion":{"major":2,"minor":0},"sequenceNumber":0,"timestamp":"2024-06-13T17:23:16Z"}
{"testRunArtifact":{"testRunStart":{"name":"ocp_diag_ssd_osv","version":"1.0","commandLine":"ocp_diag_ssd_osv","parameters":{},"dutInfo":{"dutInfoId":"1","name":"dut_ssd","metadata":{},"platformInfos":[],"hardwareInfos":[{"hardwareInfoId":"0","computerSystem":"","name":"MZWLR3T8HBLS-00AGG","location":"/dev/nvme0","odataId":"","partNumber":"","serialNumber":"S6HRNA0T756288","manager":"","manufacturer":"","manufacturerPartNumber":"","partType":"Storage","version":"MPK9AP5Q","revision":""},{"hardwareInfoId":"1","computerSystem":"","name":"SSSTC CL4-8D512","location":"/dev/nvme1","odataId":"","partNumber":"","serialNumber":"0023394006L6","manager":"","manufacturer":"","manufacturerPartNumber":"","partType":"Storage","version":"EV22002 ","revision":""}],"softwareInfos":[]},"metadata":{}}},"sequenceNumber":2,"timestamp":"2024-06-13T17:23:16Z"}
{"testStepArtifact":{"testStepStart":{"name":"nvme-identify-controller-check"},"testStepId":"0"},"sequenceNumber":3,"timestamp":"2024-06-13T17:23:16Z"}
{"testStepArtifact":{"diagnosis":{"verdict":"nvme-identify-controller-command-pass","type":"PASS","message":"NVMe Identify Controller command has been sent successfully.","hardwareInfoId":"0"},"testStepId":"0"},"sequenceNumber":4,"timestamp":"2024-06-13T17:23:16Z"}
{"testStepArtifact":{"diagnosis":{"verdict":"nvme-identify-controller-command-pass","type":"PASS","message":"NVMe Identify Controller command has been sent successfully.","hardwareInfoId":"1"},"testStepId":"0"},"sequenceNumber":5,"timestamp":"2024-06-13T17:23:16Z"}
{"testStepArtifact":{"testStepEnd":{"status":"COMPLETE"},"testStepId":"0"},"sequenceNumber":6,"timestamp":"2024-06-13T17:23:16Z"}
{"testStepArtifact":{"testStepStart":{"name":"random-read-4k(for 300 seconds)"},"testStepId":"1"},"sequenceNumber":7,"timestamp":"2024-06-13T17:23:16Z"}
{"testStepArtifact":{"diagnosis":{"verdict":"nvme-identify-namespace-command-pass","type":"PASS","message":"NVMe Identify Namespace command has been sent successfully.","hardwareInfoId":"0"},"testStepId":"1"},"sequenceNumber":8,"timestamp":"2024-06-13T17:28:16Z"}
{"testStepArtifact":{"diagnosis":{"verdict":"random-io-test-for-300-seconds-pass","type":"PASS","message":"Random I/O Test has been performed successfully.","hardwareInfoId":"0"},"testStepId":"1"},"sequenceNumber":9,"timestamp":"2024-06-13T17:28:16Z"}
{"testStepArtifact":{"diagnosis":{"verdict":"nvme-identify-namespace-command-pass","type":"PASS","message":"NVMe Identify Namespace command has been sent successfully.","hardwareInfoId":"1"},"testStepId":"1"},"sequenceNumber":10,"timestamp":"2024-06-13T17:28:16Z"}
{"testStepArtifact":{"diagnosis":{"verdict":"random-io-test-for-300-seconds-pass","type":"PASS","message":"Random I/O Test has been performed successfully.","hardwareInfoId":"1"},"testStepId":"1"},"sequenceNumber":11,"timestamp":"2024-06-13T17:28:16Z"}
{"testStepArtifact":{"testStepEnd":{"status":"COMPLETE"},"testStepId":"1"},"sequenceNumber":12,"timestamp":"2024-06-13T17:28:16Z"}
{"testStepArtifact":{"testStepStart":{"name":"nvme-smart-check"},"testStepId":"2"},"sequenceNumber":13,"timestamp":"2024-06-13T17:28:16Z"}
{"testStepArtifact":{"diagnosis":{"verdict":"nvme-get-log-page-smart-health-info-command-pass","type":"PASS","message":"NVMe Get Log Page command has been sent successfully.","hardwareInfoId":"0"},"testStepId":"2"},"sequenceNumber":14,"timestamp":"2024-06-13T17:28:16Z"}
{"testStepArtifact":{"measurement":{"name":"check-capacitor-status","unit":"Bit","hardwareInfoId":"0","validators":[{"name":"nvme_smart_critical_warning_bit_4","type":"NOT_EQUAL","value":false}],"metadata":{},"value":false},"testStepId":"2"},"sequenceNumber":15,"timestamp":"2024-06-13T17:28:16Z"}
{"testStepArtifact":{"measurement":{"name":"check-read-only-mode","unit":"Bit","hardwareInfoId":"0","validators":[{"name":"nvme_smart_critical_warning_bit_3","type":"NOT_EQUAL","value":false}],"metadata":{},"value":false},"testStepId":"2"},"sequenceNumber":16,"timestamp":"2024-06-13T17:28:16Z"}
{"testStepArtifact":{"measurement":{"name":"check-available-spare","unit":"Percentage","hardwareInfoId":"0","validators":[{"name":"nvme_smart_available_spare_100_percent","type":"EQUAL","value":100}],"metadata":{},"value":100},"testStepId":"2"},"sequenceNumber":17,"timestamp":"2024-06-13T17:28:16Z"}
{"testStepArtifact":{"measurement":{"name":"check-percentage-used","unit":"Percentage","hardwareInfoId":"0","validators":[{"name":"nvme_smart_percentage_used_100_percent","type":"LESS_THAN","value":100}],"metadata":{},"value":0},"testStepId":"2"},"sequenceNumber":18,"timestamp":"2024-06-13T17:28:16Z"}
{"testStepArtifact":{"diagnosis":{"verdict":"nvme-get-log-page-smart-health-info-command-pass","type":"PASS","message":"NVMe Get Log Page command has been sent successfully.","hardwareInfoId":"1"},"testStepId":"2"},"sequenceNumber":19,"timestamp":"2024-06-13T17:28:16Z"}
{"testStepArtifact":{"measurement":{"name":"check-capacitor-status","unit":"Bit","hardwareInfoId":"1","validators":[{"name":"nvme_smart_critical_warning_bit_4","type":"NOT_EQUAL","value":false}],"metadata":{},"value":false},"testStepId":"2"},"sequenceNumber":20,"timestamp":"2024-06-13T17:28:16Z"}
{"testStepArtifact":{"measurement":{"name":"check-read-only-mode","unit":"Bit","hardwareInfoId":"1","validators":[{"name":"nvme_smart_critical_warning_bit_3","type":"NOT_EQUAL","value":false}],"metadata":{},"value":false},"testStepId":"2"},"sequenceNumber":21,"timestamp":"2024-06-13T17:28:16Z"}
{"testStepArtifact":{"measurement":{"name":"check-available-spare","unit":"Percentage","hardwareInfoId":"1","validators":[{"name":"nvme_smart_available_spare_100_percent","type":"EQUAL","value":100}],"metadata":{},"value":100},"testStepId":"2"},"sequenceNumber":22,"timestamp":"2024-06-13T17:28:16Z"}
{"testStepArtifact":{"measurement":{"name":"check-percentage-used","unit":"Percentage","hardwareInfoId":"1","validators":[{"name":"nvme_smart_percentage_used_100_percent","type":"LESS_THAN","value":100}],"metadata":{},"value":0},"testStepId":"2"},"sequenceNumber":23,"timestamp":"2024-06-13T17:28:16Z"}
{"testStepArtifact":{"testStepEnd":{"status":"COMPLETE"},"testStepId":"2"},"sequenceNumber":24,"timestamp":"2024-06-13T17:28:16Z"}
{"testRunArtifact":{"testRunEnd":{"status":"COMPLETE","result":"PASS"}},"sequenceNumber":25,"timestamp":"2024-06-13T17:28:16Z"}
```

## Customizing Test Sequence
Customize OcpDiagSsdApp::RunDiagnostic() method in ocpdiag/core/examples/ocp_diag_ssd_osv/app/ocp_diag_ssd_osv_app.cc as per your application's requirements. 

You can inject multiple StorageDiagnosticProvider instances into DiagnosticManager. Each StorageDiagnosticProvider performs a specific type of diagnostic. For example, IoDiagnosticProvider supports I/O test, while NVMeCheckSmartDiagnosticProvider supports NVMe SMART Health check test.

**Warning**
Do not execute write test on the drive on which OS is installed.

```C++
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
```

### Create a custom StorageDiagnosticProvider
Create a new C++ class that implements the StorageDiagnosticProviderInterface interface.
- Override the std::vector<std::shared_ptr<DiagnosticLogBase>> Diagnose(std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfos) method to perform the diagnostic logic.
- Override the std::string GetDiagnosisName() method to set the name of the diagnostic logic.
- Override the bool Supports(std::shared_ptr<ocp_diag_ssd_osv::StorageInfoBase> storageInfo) to tell this StorageDiagnosticProvider supports the provided storageInfo or not.

## OSS Notice
This application is Copyright 2024 Samsung Electornics. All Rights Reserved.

This application uses Open Source Software (OSS). 

### BitInt
* Arbitrary-sized integer class for C++.
* Version: 0.4.0-dev
* Author: Syed Faheel Ahmad (faheel@live.in)
* Project on GitHub: https://github.com/faheel/BigInt