// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCPDIAG_CORE_LIB_JSON_PROTO_HELPERS_H_
#define OCPDIAG_CORE_LIB_JSON_PROTO_HELPERS_H_

#include "google/protobuf/message.h"
#include "absl/status/status.h"
#include "google/protobuf/io/zero_copy_stream.h"

namespace ocpdiag {

// Implement missing json_utils.h function to take an input JSON stream and
// merge it (or copy it) into an existing message type.
// Unlike the normal conversions, this is safe for an empty input.
// If `overwrite` is true, then the output message will be overwritten with the
// contents of the JSON stream, otherwise they will be merged. If the
// json_stream is empty then the output will not be mutated regardless.
absl::Status CombineFromJsonStream(google::protobuf::io::ZeroCopyInputStream* json_stream,
                                   google::protobuf::Message* output, bool overwrite);

// Parses a JSON file containing a proto message.
absl::Status JsonFileToMessage(const char* file_path, google::protobuf::Message* output);

}  // namespace ocpdiag

#endif  // OCPDIAG_CORE_LIB_JSON_PROTO_HELPERS_H_
