// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <cstdint>
#include <utility>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "ecclesia/lib/smbios/memory_device.h"
#include "ecclesia/lib/smbios/reader.h"
#include "ocpdiag/core/compat/status_macros.h"
#include "ocpdiag/core/hwinterface/backends/host/host_backend.h"
#include "ocpdiag/core/hwinterface/backends/lib/hw_info.h"
#include "ocpdiag/core/hwinterface/backends/lib/utils.h"
#include "ocpdiag/core/hwinterface/identifier.pb.h"
#include "ocpdiag/core/hwinterface/memory.pb.h"
#include "ocpdiag/core/hwinterface/service.pb.h"

namespace ocpdiag::hwinterface::internal {

constexpr int kNumOfBytesInKilobyte = 1024;
constexpr int kNumOfBytesInMegabyte = 1048576;
constexpr int kFlagMemorySizeInExtendedSize = 0x7FFF;
constexpr int kFlagMemorySizeUnknown = 0xFFFF;
constexpr int kMemorySizeInKilobyteMask = 0x8000;

namespace {

memory::FwMemoryType getFwMemoryType(int type_code) {
  // According to DMFT SMBIOS Reference Specification Table 76.
  //
  static const auto* const kIntToFwMemoryType =
      new absl::flat_hash_map<int, memory::FwMemoryType>{
          {0x00, memory::FW_MEMORYTYPE_INVALID},
          {0x01, memory::FW_MEMORYTYPE_OTHER},
          {0x02, memory::FW_MEMORYTYPE_UNKNOWN},
          {0x03, memory::FW_MEMORYTYPE_DRAM},
          {0x04, memory::FW_MEMORYTYPE_EDRAM},
          {0x05, memory::FW_MEMORYTYPE_VRAM},
          {0x06, memory::FW_MEMORYTYPE_SRAM},
          {0x07, memory::FW_MEMORYTYPE_RAM},
          {0x08, memory::FW_MEMORYTYPE_ROM},
          {0x09, memory::FW_MEMORYTYPE_FLASH},
          {0x0A, memory::FW_MEMORYTYPE_EEPROM},
          {0x0B, memory::FW_MEMORYTYPE_FEPROM},
          {0x0C, memory::FW_MEMORYTYPE_EPROM},
          {0x0D, memory::FW_MEMORYTYPE_CDRAM},
          {0x0E, memory::FW_MEMORYTYPE_3DRAM},
          {0x0F, memory::FW_MEMORYTYPE_SDRAM},
          {0x10, memory::FW_MEMORYTYPE_SGRAM},
          {0x11, memory::FW_MEMORYTYPE_RDRAM},
          {0x18, memory::FW_MEMORYTYPE_DDR3},
          {0x1A, memory::FW_MEMORYTYPE_DDR4},
          {0x22, memory::FW_MEMORYTYPE_DDR5},
      };
  if (const auto iter = kIntToFwMemoryType->find(type_code);
      iter != kIntToFwMemoryType->end()) {
    return iter->second;
  }
  return memory::FW_MEMORYTYPE_UNKNOWN;
}

}  // namespace

absl::StatusOr<GetMemoryInfoResponse> HostBackend::GetMemoryInfo(
    const GetMemoryInfoRequest& req) {
  GetMemoryInfoResponse resp;
  ASSIGN_OR_RETURN(ecclesia::SmbiosReader * reader, GetSmbiosReader());

  int index = -1;

  for (const auto& memory_device : reader->GetAllMemoryDevices()) {
    ecclesia::MemoryDeviceStructureView message_view =
        memory_device.GetMessageView();

    index++;

    memory::Info& info = (*resp.mutable_dimm_infos())[index];
    if (InfoTypeHave(req.info_types(), memory::InfoType::PRESENCE)) {
      memory::Presence& presence = *info.mutable_presence();
      // Currently set present_in_slot and present_in_firmware as the same.
      if (message_view.size().Read() == 0) {
        presence.set_present_in_slot(false);
        presence.set_present_in_firmware(false);
      } else {
        presence.set_present_in_slot(true);
        presence.set_present_in_firmware(true);
      }
    }
    if (InfoTypeHave(req.info_types(), memory::InfoType::SIZE)) {
      const uint64_t size = message_view.size().Read();
      if (size == kFlagMemorySizeInExtendedSize) {
        // If the size is 32 GB-1 MB or greater, the field value is 7FFFh and
        // the actual size is stored in the Extended Size field.
        if (message_view.has_extended_size().ValueOr(false)) {
          info.set_size_bytes((uint64_t)message_view.extended_size().Read() *
                              kNumOfBytesInMegabyte);
          // Extended size field represents the size of the memory device in
          // megabytes.
        }
        // else: size is unknown. Leave size_bytes as 0.
      } else if (size == kFlagMemorySizeUnknown) {
        // size is unknown
        info.set_size_bytes(0);
      } else if (size & kMemorySizeInKilobyteMask) {
        //  If the most-significant bit is 1, the value is specified in
        //  kilobyte units; if the most-significant bit is 0, the value is
        //  specified in megabyte units. message_view.size() is originally a
        //  two-bytes value.
        info.set_size_bytes((size & ~kMemorySizeInKilobyteMask) *
                            kNumOfBytesInKilobyte);
      } else {
        // Size is specified in megabyte
        info.set_size_bytes(size * kNumOfBytesInMegabyte);
      }
    }
    if (InfoTypeHave(req.info_types(), memory::InfoType::DIMM_IDENTIFIER)) {
      Identifier& id = *info.mutable_identifier();
      id.set_type("dimm");
      id.set_name(absl::StrCat("DIMM", index));
      PopulateId(id);
    }
    if (InfoTypeHave(req.info_types(), memory::InfoType::DIMM_LOCATION)) {
      memory::Location& location = *info.mutable_location();
      location.set_slot_name(absl::StrCat("DIMM", index));
    }
    if (InfoTypeHave(req.info_types(), memory::InfoType::FW_MEMORY_TYPE)) {
      info.set_fw_memory_type(
          getFwMemoryType((int)message_view.memory_type().Read()));
    }
    if (InfoTypeHave(req.info_types(), memory::InfoType::SPEED)) {
      memory::Speed& speed = *info.mutable_speed();
      if (message_view.has_speed().ValueOr(false)) {
        speed.set_firmware_speed_mhz(message_view.speed().Read());
      }
      if (message_view.has_configured_memory_clock_speed().ValueOr(false)) {
        speed.set_operating_speed_mhz(
            message_view.configured_memory_clock_speed().Read());
      }
    }
    if (InfoTypeHave(req.info_types(), memory::InfoType::JEDEC)) {
      if (message_view.has_module_manufacturer_id().ValueOr(false)) {
        memory::Jedec& jedec = *info.mutable_jedec();
        // No table id listed in smbios.
        jedec.set_manufacturer_index(
            message_view.module_manufacturer_id().Read());
      }
    }
  }

  return resp;
}

}  // namespace ocpdiag::hwinterface::internal
