// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


#include <cstdint>
#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "ecclesia/lib/smbios/memory_device.h"
#include "ecclesia/lib/smbios/memory_device_mapped_address.h"
#include "ecclesia/lib/smbios/reader.h"
#include "ecclesia/lib/smbios/structures.emb.h"
#include "ocpdiag/core/compat/status_macros.h"
#include "ocpdiag/core/hwinterface/backends/host/host_backend.h"
#include "ocpdiag/core/hwinterface/backends/lib/hw_info.h"
#include "ocpdiag/core/hwinterface/identifier.pb.h"
#include "ocpdiag/core/hwinterface/service.pb.h"

namespace ocpdiag::hwinterface::internal {

constexpr uint32_t kExtendedAddressMarker = 0xFFFFFFFF;
constexpr int kBytesPerKilobyte = 1024;

bool IsAddressWithinMap(uint64_t address,
                        const ecclesia::MemoryDeviceMappedAddressStructureView&
                            mapped_address_view) {
  uint64_t start_address =
      mapped_address_view.starting_address().Read() == kExtendedAddressMarker
          ? mapped_address_view.extended_starting_address().Read()
          : mapped_address_view.starting_address().Read();
  uint64_t end_address =
      mapped_address_view.ending_address().Read() == kExtendedAddressMarker
          ? mapped_address_view.extended_ending_address().Read()
          : mapped_address_view.ending_address().Read();
  start_address *= kBytesPerKilobyte;
  end_address *= kBytesPerKilobyte;

  return address >= start_address && address <= end_address;
}

absl::StatusOr<MemoryConvertResponse> HostBackend::MemoryConvert(
    const MemoryConvertRequest& req) {
  MemoryConvertResponse resp;
  ASSIGN_OR_RETURN(ecclesia::SmbiosReader * reader, GetSmbiosReader());

  uint16_t device_handle = 0;
  for (const auto& mapped_address :
       reader->GetAllMemoryDeviceMappedAddresses()) {
    ecclesia::MemoryDeviceMappedAddressStructureView mapped_address_view =
        mapped_address.GetMessageView();
    if (IsAddressWithinMap(req.physical_address(), mapped_address_view)) {
      device_handle = mapped_address_view.memory_device_handle().Read();
      break;
    }
  }
  if (device_handle == 0) {
    return absl::InvalidArgumentError(
        absl::StrFormat("Unable to find physical address 0x%x in the Memory "
                        "Device Mapped Address table.",
                        req.physical_address()));
  }

  for (const auto& memory_device : reader->GetAllMemoryDevices()) {
    ecclesia::MemoryDeviceStructureView memory_device_view =
        memory_device.GetMessageView();
    if (memory_device_view.handle().Read() == device_handle) {
      Identifier& identifier = *resp.add_identifiers();
      identifier.set_name(std::string(memory_device.GetString(
          memory_device_view.device_locator_snum().Read())));
      identifier.set_type("dimm");
      PopulateId(identifier);
      return resp;
    }
  }

  return absl::NotFoundError(absl::StrFormat(
      "Unable to find memory device with handle 0x%x.", device_handle));
}

}  // namespace ocpdiag::hwinterface::internal
