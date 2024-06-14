// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <vector>
#include "gtest/gtest.h"
#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/utils/buffer_utils.h"

namespace ocp_diag_ssd_osv {

TEST(BufferUtilsTest, IsBufferAllZeroTest_WithNonAllZeroBuffer) {
    std::vector<unsigned char> buff = {1, 1, 1, 0, 0, 0, 0};
    ASSERT_FALSE(BufferUtils::IsBufferAllZero(buff, 0));
}

TEST(BufferUtilsTest, IsBufferAllZeroTest_WithNonAllZeroBufferWithStartIndex) {
    std::vector<unsigned char> buff = {1, 1, 1, 0, 0, 0, 0};
    ASSERT_TRUE(BufferUtils::IsBufferAllZero(buff, 3));
}

TEST(BufferUtilsTest, SetNumberToByteBufferLittleEndianTest) {
    std::vector<unsigned char> buff = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    BufferUtils::SetNumberToByteBufferLittleEndian(buff, 2, 4, 0xABCDEF99);

    ASSERT_EQ(buff[0], 0xFF);
    ASSERT_EQ(buff[1], 0xFF);
    ASSERT_EQ(buff[2], 0x99);
    ASSERT_EQ(buff[3], 0xEF);
    ASSERT_EQ(buff[4], 0xCD);
    ASSERT_EQ(buff[5], 0xAB);
    ASSERT_EQ(buff[6], 0xFF);
    ASSERT_EQ(buff[7], 0xFF);
}

TEST(BufferUtilsTest, GetNumberFromByteBufferBigEndianTest) {
    std::vector<unsigned char> buff = {0xFF, 0xFF, 64, 120, 125, 1, 0xFF, 0xFF};
    ASSERT_EQ(BufferUtils::GetNumberFromByteBufferLittleEndian(buff, 2, 4), 25000000);
}

TEST(BufferUtilsTest, GetBigIntFromByteBufferTest) {
    std::vector<unsigned char> buff = {0xFF, 0xFF, 0x77, 0x8A, 0x1D, 0x55, 0xD8, 0x4A, 0x70, 0xBE, 0x5D, 0x04, 0x1B, 0xEF, 0xEE, 0x4F, 0x6C, 0xB5, 0xFF, 0xFF};
    ASSERT_EQ(BufferUtils::GetBigIntFromByteBufferLittleEndian(buff, 2, 16), BigInt("241152656538452480219512957714718100087"));
}

TEST(BufferUtilsTest, GetStringFromDataBuffer_RemoveSpace) {
    std::vector<unsigned char> buff = {0xFF, 0xFF, 'S', 'T', 'R', 'I', 'N', 'G', ' ', ' ', 0xFF, 0xFF};
    ASSERT_EQ(BufferUtils::GetStringFromDataBuffer(buff, 2, 8), "STRING  ");
}

TEST(BufferUtilsTest, GetStringFromDataBuffer_NoRemoveSpace) {
    std::vector<unsigned char> buff = {0xFF, 0xFF, 'S', 'T', 'R', 'I', 'N', 'G', ' ', ' ', 0xFF, 0xFF};
    ASSERT_EQ(BufferUtils::GetStringFromDataBuffer(buff, 2, 8, true), "STRING");
}

}