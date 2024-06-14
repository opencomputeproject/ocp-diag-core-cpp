// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "diagnostic_measurement_log.h"
#include <cassert>

namespace ocp_diag_ssd_osv {

ocpdiag::results::Validator StorageTestMeasurementValidator::ToOcpDiagValidator() const {

    ocpdiag::results::ValidatorType validatorType;

    switch(GetStorageTestValidatorType())
    {
        case StorageTestValidatorType::Unspecified:
            validatorType = ocpdiag::results::ValidatorType::kUnspecified;
            break;
        case StorageTestValidatorType::Equal:
            validatorType = ocpdiag::results::ValidatorType::kEqual;
            break;
        case StorageTestValidatorType::NotEqual:
            validatorType = ocpdiag::results::ValidatorType::kNotEqual;
            break;
        case StorageTestValidatorType::LessThan:
            validatorType = ocpdiag::results::ValidatorType::kLessThan;
            break;
        case StorageTestValidatorType::LessThanOrEqual:
            validatorType = ocpdiag::results::ValidatorType::kLessThanOrEqual;
            break;
        case StorageTestValidatorType::GreaterThan:
            validatorType = ocpdiag::results::ValidatorType::kGreaterThan;
            break;
        case StorageTestValidatorType::GreaterThanOrEqual:
            validatorType = ocpdiag::results::ValidatorType::kGreaterThanOrEqual;
            break;
        case StorageTestValidatorType::RegexMatch:
            validatorType = ocpdiag::results::ValidatorType::kRegexMatch;
            break;
        case StorageTestValidatorType::RegexNoMatch:
            validatorType = ocpdiag::results::ValidatorType::kRegexNoMatch;
            break;
        case StorageTestValidatorType::InSet:
            validatorType = ocpdiag::results::ValidatorType::kInSet;
            break;
        case StorageTestValidatorType::NotInSet:
            validatorType = ocpdiag::results::ValidatorType::kNotInSet;
            break;
        default:
            assert(false && "Invalid StorageTestValidatorType");
            break;
    }

    return ocpdiag::results::Validator{
        .type = validatorType,
        .value = GetValue(),
        .name = GetValidatorName()
    };
}

ocpdiag::results::Measurement ocp_diag_ssd_osv::DiagnosticMeasurementLog::ToOcpDiagMeasurement() {

    std::vector<ocpdiag::results::Validator> validators;

    std::for_each(GetValidators().begin(),
                  GetValidators().end(),
                  [&validators](const StorageTestMeasurementValidator& validator){
        validators.push_back(validator.ToOcpDiagValidator());
    });
    
    return ocpdiag::results::Measurement{
        .name = GetName(),
        .unit = GetUnit(),
        .hardware_info = GetRegisteredHardwareInfoRef(),
        .validators = validators,
        .value = GetActualValue(),
        .metadata_json = GetMetadataJson()
    };
}


}

