// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_BUFFER_UTILS_H
#define OCP_DIAG_SSD_OSV_BUFFER_UTILS_H

#include <vector>
#include <string>
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/utils/BigInt.hpp"

namespace ocp_diag_ssd_osv {

class BufferUtils {
public:
    // It returns true if all elements of given std::vector are all 0.
    static bool                         IsBufferAllZero(const std::vector<unsigned char>& buffer, size_t startIndex = 0);
    static void                         SetNumberToByteBufferLittleEndian(std::vector<unsigned char>& buf, int offsetByte, int lengthBytes, unsigned long long value);
    static unsigned long long           GetNumberFromByteBufferLittleEndian(const std::vector<unsigned char>& buf, int offsetByte, int lengthBytes);
    static std::string                  GetStringFromDataBuffer(const std::vector<unsigned char>& vec, int offset, int length, bool removeSpace = false);
    static BigInt                       GetBigIntFromByteBufferLittleEndian(const std::vector<unsigned char>& buf, int offsetByte, int lengthBytes);
};

}

#endif //OCP_DIAG_SSD_OSV_BUFFER_UTILS_H
