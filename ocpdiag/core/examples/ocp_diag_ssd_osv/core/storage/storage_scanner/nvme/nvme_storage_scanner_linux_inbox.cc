// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <regex>
#include <dirent.h>
#include "nvme_storage_scanner_linux_inbox.h"

namespace ocp_diag_ssd_osv {

std::vector<std::string> NVMeStorageScannerLinuxInbox::GetAllNVMeStorageDeviceCandidates() {
    std::vector<std::string> devPaths;

    std::regex nvmeDiskRegex(nvmeDeviceCandidateDeviceRegex_);

    DIR* directory = ::opendir(DEVICE_ROOT_DIR);
    if(directory != nullptr)
    {
        struct dirent* directPtr = nullptr;

        while((directPtr = readdir(directory)))
        {
            std::string devName(directPtr->d_name);

            if(regex_match(devName, nvmeDiskRegex))
            {
                devPaths.push_back(DEVICE_ROOT_DIR + devName);
            }
        }
        closedir(directory);
    }

    std::sort(devPaths.begin(), devPaths.end());

    return devPaths;
}

}



