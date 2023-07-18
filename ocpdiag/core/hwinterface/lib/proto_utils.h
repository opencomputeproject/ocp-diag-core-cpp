// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_HWINTERFACE_LIB_PROTO_UTILS_H_
#define OCPDIAG_CORE_HWINTERFACE_LIB_PROTO_UTILS_H_

#include "google/protobuf/descriptor.h"
#include "google/protobuf/generated_enum_reflection.h"

namespace ocpdiag::hwinterface {

template <typename E>
class ProtoEnumView {
 public:
  ProtoEnumView() { enum_desc_ = google::protobuf::GetEnumDescriptor<E>(); }

  class Iterator
      : public std::iterator<std::input_iterator_tag,  // iterator_category
                             E,                        // value_type
                             std::ptrdiff_t,           // difference_type
                             const E*,                 // pointer
                             E                         // reference
                             > {
   public:
    Iterator(const google::protobuf::EnumDescriptor* enum_desc)
        : index_(0), enum_desc_(enum_desc) {}
    Iterator(int index, const google::protobuf::EnumDescriptor* enum_desc)
        : index_(index), enum_desc_(enum_desc) {}
    E operator*() {
      if (index_ >= enum_desc_->value_count()) {
        // Undefined bahavior.
        return static_cast<E>(enum_desc_->value(0)->number());
      }
      return static_cast<E>(enum_desc_->value(index_)->number());
    }
    bool operator!=(const Iterator& rhs) { return index_ != rhs.index_; }
    void operator++() { ++index_; }

   private:
    int index_;
    const google::protobuf::EnumDescriptor* enum_desc_;
  };

  auto begin() const { return Iterator(enum_desc_); }
  auto end() const { return Iterator(enum_desc_->value_count(), enum_desc_); }

 private:
  const google::protobuf::EnumDescriptor* enum_desc_;
};

}  // namespace ocpdiag::hwinterface

#endif  // OCPDIAG_CORE_HWINTERFACE_LIB_PROTO_UTILS_H_
