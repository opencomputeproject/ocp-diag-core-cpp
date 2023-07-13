// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/lib/identifier_utils.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <tuple>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "ocpdiag/core/compat/status_macros.h"
#include "ocpdiag/core/hwinterface/identifier.pb.h"
#include "re2/re2.h"

namespace ocpdiag::hwinterface {

absl::StatusOr<IdentifierMatcher> IdentifierMatcher::Create(
    const Filter& filter) {
  IdentifierMatcher out(filter);

  if (filter.has_id_regex()) {
    const Identifier& regex = filter.id_regex();
    for (int i = 0; i < ABSL_ARRAYSIZE(kIdFields); ++i) {
      const std::string& expression = (regex.*kIdFields[i])();
      if (!expression.empty()) {
        out.regexes_[i] = std::make_unique<RE2>(expression);
        if (!out.regexes_[i]->ok()) {
          return absl::InvalidArgumentError(
              absl::StrCat("Couldn't create regex \"", expression,
                           "\": ", out.regexes_[i]->error()));
        }
      }
    }
  }
  return out;
}

bool IdentifierMatcher::Empty() const {
  return absl::c_all_of(regexes_, [](const std::unique_ptr<RE2>& regex) {
    return regex == nullptr;
  });
}

bool IdentifierMatcher::AndAll(const Identifier& id) const {
  for (int i = 0; i < ABSL_ARRAYSIZE(kIdFields); ++i) {
    if (const RE2* regex = regexes_[i].get();
        regex != nullptr && !RE2::PartialMatch((id.*kIdFields[i])(), *regex)) {
      return false;
    }
  }
  return true;
}

bool IdentifierMatcher::OrAll(const Identifier& id) const {
  for (int i = 0; i < ABSL_ARRAYSIZE(kIdFields); ++i) {
    if (const RE2* regex = regexes_[i].get();
        regex != nullptr && RE2::PartialMatch((id.*kIdFields[i])(), *regex)) {
      return true;
    }
  }
  return false;
}

bool IdentifierMatcher::Match(const Identifier& id) const {
  if (Empty()) {
    return true;
  }
  // Set OR as the default operator.
  if (op_ == Filter::AND) {
    return AndAll(id);
  }
  return OrAll(id);
}

absl::StatusOr<IdentifierMatchersUnion> IdentifierMatchersUnion::Create(
    const google::protobuf::RepeatedPtrField<Filter>& filters) {
  std::vector<IdentifierMatcher> matchers;

  for (const auto& filter : filters) {
    ASSIGN_OR_RETURN(IdentifierMatcher matcher,
                     IdentifierMatcher::Create(filter));
    matchers.push_back(std::move(matcher));
  }
  return IdentifierMatchersUnion(std::move(matchers));
}

bool IdentifierMatchersUnion::Match(const hwinterface::Identifier& id) const {
  if (matchers_.empty()) {
    return true;
  }

  for (const auto& matcher : matchers_) {
    if (matcher.Match(id)) {
      return true;
    }
  }
  return false;
}

}  // namespace ocpdiag::hwinterface
