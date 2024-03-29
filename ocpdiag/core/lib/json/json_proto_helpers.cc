// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/lib/json/json_proto_helpers.h"

#include <fcntl.h>
#include <unistd.h>

#include <memory>
#include <string>

#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "google/protobuf/util/json_util.h"
#include "google/protobuf/util/type_resolver.h"
#include "google/protobuf/util/type_resolver_util.h"
#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "ocpdiag/core/compat/status_converters.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/io/zero_copy_stream_impl_lite.h"

namespace ocpdiag {

absl::Status CombineFromJsonStream(google::protobuf::io::ZeroCopyInputStream* json_stream,
                                   google::protobuf::Message* output, bool overwrite) {
  const std::string type_prefix = "type.googleapis.com";
  const std::string type_url =
      absl::StrCat(type_prefix, "/", output->GetDescriptor()->full_name());
  std::unique_ptr<google::protobuf::util::TypeResolver> resolver{
      google::protobuf::util::NewTypeResolverForDescriptorPool(
          type_prefix, output->GetDescriptor()->file()->pool())};
  std::string binary_proto;
  // The JSON stream parser doesn't accept an empty string.
  const void* data;
  int bytes;
  if (json_stream->Next(&data, &bytes)) {
    json_stream->BackUp(bytes);
    google::protobuf::io::StringOutputStream binary_stream{&binary_proto};
    if (absl::Status json_parsed =
            AsAbslStatus(google::protobuf::util::JsonToBinaryStream(
                resolver.get(), type_url, json_stream, &binary_stream));
        !json_parsed.ok()) {
      return json_parsed;
    }

    // Only overwrite the output when JSON was actually given.
    if (overwrite) output->Clear();
  }
  if (!output->MergeFromString(binary_proto)) {
    return absl::InternalError(
        "JSON transcoder produced invalid protobuf output.");
  }
  return absl::OkStatus();
}

// Parses a JSON file containing a proto message
absl::Status JsonFileToMessage(const char* file_path, google::protobuf::Message* output) {
  int defaults_fd = open(file_path, O_RDONLY);
  if (defaults_fd < 0) {
    return absl::NotFoundError(
        absl::StrCat("Failed to open defaults file: ", file_path));
  }
  google::protobuf::io::FileInputStream defaults_stream{defaults_fd};
  absl::Status status = CombineFromJsonStream(&defaults_stream, output, true);
  defaults_stream.Close();
  return status;
}

}  // namespace ocpdiag
