// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_DIAGNOSTIC_MEASUREMENT_LOG_H
#define OCP_DIAG_SSD_OSV_DIAGNOSTIC_MEASUREMENT_LOG_H

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/app/diagnosis/diagnostic_log/diagnostic_log_base.h"

namespace ocp_diag_ssd_osv {

enum class StorageTestValidatorType {
    Unspecified = 0,
    Equal = 1,
    NotEqual = 2,
    LessThan = 3,
    LessThanOrEqual = 4,
    GreaterThan = 5,
    GreaterThanOrEqual = 6,
    RegexMatch = 7,
    RegexNoMatch = 8,
    InSet = 9,
    NotInSet = 10,
};

//=========================================
// StorageTestMeasurementValidator
//=========================================
class StorageTestMeasurementValidator
{
public:
    StorageTestMeasurementValidator(StorageTestValidatorType storageTestValidatorType,
                                    const std::vector<ocpdiag::results::Variant> &value,
                                    const std::string& validatorName) :
            storageTestValidatorType_(storageTestValidatorType),
            value_(value),
            validatorName_(validatorName) {}
public:
    StorageTestValidatorType                        GetStorageTestValidatorType() const { return storageTestValidatorType_; }
    const std::vector<ocpdiag::results::Variant>&   GetValue() const { return value_; }
    const std::string&                              GetValidatorName() const { return validatorName_; }

    ocpdiag::results::Validator                     ToOcpDiagValidator() const;

protected:
    StorageTestValidatorType                        storageTestValidatorType_;
    std::vector<ocpdiag::results::Variant>          value_;
    std::string                                     validatorName_;
};

//=========================================
// DiagnosticMeasurementLog
//=========================================
class DiagnosticMeasurementLog : public DiagnosticLogBase {
public:
    DiagnosticMeasurementLog(const std::shared_ptr<StorageInfoBase>& storageInfo,
                             const std::string& name,
                             const std::string& unit,
                             const std::vector<StorageTestMeasurementValidator>& validators,
                             ocpdiag::results::Variant actualValue,
                             const std::string& metadata_json = "")
     : DiagnosticLogBase(StorageTestLogType::Measurement, storageInfo),
       name_(name), unit_(unit), validators_(validators), actualValue_(std::move(actualValue)), metadata_json_(metadata_json) {

    }
    ~DiagnosticMeasurementLog() override = default;

public:
    const std::string&                                  GetName() const { return name_; }
    const std::string&                                  GetUnit() const { return unit_; }
    const std::vector<StorageTestMeasurementValidator>& GetValidators() const { return validators_; }
    const ocpdiag::results::Variant&                    GetActualValue() const { return actualValue_; }
    const std::string&                                  GetMetadataJson() const { return metadata_json_; }

    ocpdiag::results::Measurement                       ToOcpDiagMeasurement();

protected:
    std::string                                         name_;
    std::string                                         unit_;
    std::vector<StorageTestMeasurementValidator>        validators_;
    ocpdiag::results::Variant                           actualValue_;
    std::string                                         metadata_json_;
};

}

#endif //OCP_DIAG_SSD_OSV_DIAGNOSTIC_MEASUREMENT_LOG_H
