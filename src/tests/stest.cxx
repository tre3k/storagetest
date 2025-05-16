/*
 * Copyright (c) 2025 Kirill Pshenichnyi <pshcyrill@mail.ru>
 *
 * This file is part of storagetest.
 *
 * storagetest is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * storagetest is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with storagetest.  If not, see <https://www.gnu.org/licenses/>.
 *
 *    Author: Kirill Pshenichnyi <pshcyrill@mail.ru>
 *
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <random>

#include "config.h"

extern "C" {
#include "fileinformation.h"
}

TEST(FileInformation, fill_structure) {
        std::cout << "FileInformation size on current system: " << fiSize()
                  << " bytes." << std::endl;
        FileInformation *fi = fiInit();
        fiFree(fi);

        unsigned int size = sizeof(char *) + 3 * sizeof(size_t) +
                            sizeof(unsigned char *) + sizeof(unsigned int) +
                            sizeof(unsigned char);

        ASSERT_GE(fiSize(), size);

        char *path = (char *)("Some/Path/To/File");
        size_t file_size {4096}, block_size {1024}, progress {512};
        unsigned char byte = 0x8c;
        unsigned char sha_sum[SHA_SUM_LENGTH];

        std::mt19937 engine;
        engine.seed(std::time(nullptr));
        for (int i = 0; i < SHA_SUM_LENGTH; i++) {
                auto rnd_val = 0xff * engine() / (engine.max() - engine.min());
                sha_sum[i] = rnd_val;
        }

        fiSetPath(path, fi);
        fiSetShaSum(sha_sum, fi);
        fiSetContentType(RANDOM, fi);

        fiSetFileSize(file_size, fi);
        fiSetBlockSize(block_size, fi);
        fiSetProgress(progress, fi);
        fiSetContentConstant(byte, fi);

        ASSERT_STREQ(fiGetPath(fi), path);
        for (int i = 0; i < SHA_SUM_LENGTH; i++) {
                ASSERT_EQ(fiGetShaSum(fi)[i], sha_sum[i]);
        }
        ASSERT_EQ(fiGetContentType(fi), RANDOM);
        ASSERT_EQ(fiGetFileSize(fi), file_size);
        ASSERT_EQ(fiGetBlockSize(fi), block_size);
        ASSERT_EQ(fiGetProgress(fi), progress);
        ASSERT_EQ(fiGetContentConstant(fi), byte);

        fiFree(fi);
}

int main(int argc, char *argv[]) {
        ::testing::InitGoogleTest(&argc, argv);
        ::testing::InitGoogleMock(&argc, argv);

        return RUN_ALL_TESTS();
}
