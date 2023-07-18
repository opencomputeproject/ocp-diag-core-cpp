// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/backends/host/host_test_utils.h"

#include <string>
#include <utility>

#include "absl/log/log.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "ecclesia/lib/apifs/apifs.h"
#include "ocpdiag/core/testing/file_utils.h"

namespace ocpdiag::hwinterface::host::testing {

using ::ecclesia::ApifsFile;
using ::ocpdiag::testutils::GetDataDependencyFilepath;

std::string GetTestDataContentsOrDie(absl::string_view filename) {
  constexpr absl::string_view kTestCaseDirPath =
      "ocpdiag/core/hwinterface/backends/host/testdata";

  std::string filepath =
      GetDataDependencyFilepath(absl::StrCat(kTestCaseDirPath, "/", filename));

  ApifsFile file(filepath);
  absl::StatusOr<std::string> out = file.Read();
  if (!out.ok()) {
    LOG(FATAL) << "File read failed " << filepath << "status: " << out.status();
  }
  return *std::move(out);
}

}  // namespace ocpdiag::hwinterface::host::testing
