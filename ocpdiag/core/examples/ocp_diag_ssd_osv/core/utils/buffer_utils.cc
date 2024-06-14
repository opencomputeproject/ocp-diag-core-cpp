// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <cassert>
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/utils/buffer_utils.h"

namespace ocp_diag_ssd_osv {

bool BufferUtils::IsBufferAllZero(const std::vector<unsigned char> &buffer, size_t startIndex) {
    for (size_t i = startIndex; i < buffer.size(); ++i)
    {
        if ((buffer[i] != 0))
        {
            return false;
        }
    }

    return true;
}

void BufferUtils::SetNumberToByteBufferLittleEndian(std::vector<unsigned char> &buf, int offsetByte,
                                                    int lengthBytes, unsigned long long int value) {

    long long startIdx = offsetByte;
    long long endIdx = startIdx + lengthBytes;

    assert(lengthBytes <= 8); // uint64 is 8Byte
    assert(endIdx < static_cast<long long>(buf.size()) && startIdx >= 0);

    unsigned long long result = value;

    for (long long idx = startIdx; idx <= endIdx; idx++)
    {
        buf[idx] = result % 256;
        result /= 256;

        if(result == 0)
            break;
    }

}

unsigned long long BufferUtils::GetNumberFromByteBufferLittleEndian(const std::vector<unsigned char> &buf,
                                                                                  int offsetByte,
                                                                                  int lengthBytes) {
    long long startIdx = static_cast<size_t>(offsetByte + lengthBytes - 1); // to prevent from underflow

    assert(lengthBytes <= 8); // uint64 is 8Byte
    assert(startIdx < static_cast<long long>(buf.size()) && startIdx >= 0);

    unsigned long long result = 0;

    for (long long idx = startIdx; idx >= offsetByte; idx--)
    {
        result = (result << 8) + buf[idx];
    }

    return result;
}

std::string BufferUtils::GetStringFromDataBuffer(const std::vector<unsigned char> &vec,
                                                 int offset,
                                                 int length,
                                                 bool removeSpace) {
    std::string str;
    long long len = static_cast<size_t>(offset) + length;

    for (long long pos = offset; pos < len; ++pos)
    {
        if (isprint(vec[pos]) == false)
            return str;

        if (removeSpace)
        {
            if (vec[pos] == ' ')
                continue;
        }

        str.push_back(vec[pos]);
    }

    return str;
}

BigInt BufferUtils::GetBigIntFromByteBufferLittleEndian(const std::vector<unsigned char> &buf,
                                                                      int offsetByte,
                                                                      int lengthBytes) {

    long long startIdx = static_cast<long long>(offsetByte + lengthBytes - 1); // to prevent from underflow
    assert(startIdx < static_cast<long long>(buf.size()) && startIdx >= 0);

    BigInt result = 0;

    for (long long idx = startIdx; idx >= offsetByte; idx--)
    {
        result = (result * 256) + buf[idx];
    }

    return result;
}

}

