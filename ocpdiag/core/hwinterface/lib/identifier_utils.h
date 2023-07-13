// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_HWINTERFACE_LIB_IDENTIFIER_UTILS_H_
#define OCPDIAG_CORE_HWINTERFACE_LIB_IDENTIFIER_UTILS_H_

#include <memory>
#include <vector>

#include "google/protobuf/repeated_field.h"
#include "absl/status/statusor.h"
#include "ocpdiag/core/hwinterface/identifier.pb.h"
#include "re2/re2.h"

namespace ocpdiag::hwinterface {

class IdentifierMatcher {
 public:
  IdentifierMatcher(const IdentifierMatcher& other) {
    op_ = other.op_;
    for (int i = 0; i < other.regexes_.size(); ++i) {
      if (other.regexes_[i] != nullptr) {
        regexes_[i] = std::make_unique<RE2>(other.regexes_[i]->pattern());
      }
    }
  }
  IdentifierMatcher& operator=(IdentifierMatcher&&) = default;

  static absl::StatusOr<IdentifierMatcher> Create(const Filter& filter);

  // Returns true when the filter id_regex is not set or all the fields in
  // id_regex are empty.
  bool Empty() const;

  // Returns whether an identifier matches the filter.
  // If the filter operator is not defined, a default OR will be applied.
  // If the filter id_regex is not set or all the fields in id_regex are empty,
  // the fuction will return true.
  bool Match(const hwinterface::Identifier& id) const;

 private:
  IdentifierMatcher(const Filter& filter) : op_(filter.op()) {}

  bool AndAll(const hwinterface::Identifier& id) const;
  bool OrAll(const hwinterface::Identifier& id) const;

  using IdentifierIdField = const std::string& (Identifier::*)() const;
  static constexpr IdentifierIdField kIdFields[] = {
      &Identifier::devpath, &Identifier::odata_id, &Identifier::name,
      &Identifier::arena,   &Identifier::type,     &Identifier::id,
  };

  Filter_Operator op_;
  // Regular expressions to match identifier fields.
  std::array<std::unique_ptr<RE2>, ABSL_ARRAYSIZE(kIdFields)> regexes_;
};

// Identifer Matchers Unin.
// Example:
//   google::protobuf::RepeatedPtrField<Filter> filters;
//   filters.Add()->mutable_id_regex()->set_name("name1");
//   filters.Add()->mutable_id_regex()->set_name("name2");
//
//   IdentifierMatchersUnion matchers =
//   IdentifierMatchersUnion::Create(filters);
//
//   Identifier id;
//   id.set_name("name1");
//   bool match = matchers.Match(id);
class IdentifierMatchersUnion {
 public:
  // Creates an IdentifierMatchersUnion from proto message "repeate Filter"
  // type.
  // Error is returned when one of the `filters` is invalid, for exmaple, regex
  // expression in Filter.id_regex can not be built.
  static absl::StatusOr<IdentifierMatchersUnion> Create(
      const google::protobuf::RepeatedPtrField<Filter>& filters);

  // Returns whether an identifier matches one of the filters.
  // For each filter,
  // If the filter operator is not defined, a default OR will be applied.
  // If the filter id_regex is not set or all the fields in id_regex are empty,
  // the fuction will return true.
  bool Match(const hwinterface::Identifier& id) const;

 private:
  IdentifierMatchersUnion(std::vector<IdentifierMatcher>&& matchers)
      : matchers_(std::move(matchers)) {}

  std::vector<IdentifierMatcher> matchers_;
};

}  // namespace ocpdiag::hwinterface

#endif  // OCPDIAG_CORE_HWINTERFACE_LIB_IDENTIFIER_UTILS_H_
