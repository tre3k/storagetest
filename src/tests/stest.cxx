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

#include <chrono>
#include <random>
#include <thread>

#include "config.h"

extern "C" {
#include <openssl/sha.h>
#include <pthread.h>

#include "fileinformation.h"
#include "status.h"
#include "storagetest.h"
}

TEST(FileInformation, fill_structure) {
        std::cout << "FileInformation size on current system: " << fiSize()
                  << " bytes." << std::endl;
        FileInformation *fi = fiInit();
        fiFree(fi);

        unsigned int size = sizeof(char *) + 3 * sizeof(size_t) +
                            sizeof(unsigned char *) + sizeof(unsigned int) +
                            sizeof(unsigned char);

        EXPECT_GE(fiSize(), size);

        char *path = (char *)("Some/Path/To/File");
        size_t file_size {4096}, block_size {1024};
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
        fiSetContentConstant(byte, fi);

        ASSERT_STREQ(fiGetPath(fi), path);
        for (int i = 0; i < SHA_SUM_LENGTH; i++) {
                ASSERT_EQ(fiGetShaSum(fi)[i], sha_sum[i]);
        }
        ASSERT_EQ(fiGetContentType(fi), RANDOM);
        ASSERT_EQ(fiGetFileSize(fi), file_size);
        ASSERT_EQ(fiGetBlockSize(fi), block_size);
        ASSERT_EQ(fiGetContentConstant(fi), byte);
}

TEST(FileInformation, initArray) {
        auto count {4};
        FileInformation **fis = fiInitArray(count);

        const char *paths[] = {"file0", "file1", "file2", "file3", "file4"};
        unsigned char content[] = {0x00, 0x01, 0x02, 0x03, 0x04};

        for (int i = 0; i < count; i++) {
                fiSetPath((char *)(paths[i]), fiGetFromArray(fis, i));
                fiSetContentConstant(content[i], fiGetFromArray(fis, i));
        }

        for (int i = 0; i < count; i++) {
                ASSERT_STREQ(fiGetPath(fiGetFromArray(fis, i)), paths[i]);
                ASSERT_EQ(fiGetContentConstant(fiGetFromArray(fis, i)),
                          content[i]);
        }

        fiFreeArray(fis);
}

TEST(status, statusInit) {
        Status *status = statusInit();

        const char *path = "Some/File/Path";
        statusSetFilePath(status, path);
        ASSERT_STREQ(path, statusGetFilePath(status));

        statusFree(status);
}

TEST(storagetest, checkShaSums) {
        std::mt19937 engine;
        unsigned char sha_sum1[SHA_SUM_LENGTH], sha_sum2[SHA_SUM_LENGTH];
        engine.seed(std::time(nullptr));
        for (int i = 0; i < SHA_SUM_LENGTH; i++) {
                auto rnd_val = 0xff * engine() / (engine.max() - engine.min());
                sha_sum1[i] = rnd_val;
                sha_sum2[i] = sha_sum1[i];
        }

        ASSERT_TRUE(checkShaSums(sha_sum1, sha_sum2));

        for (int i = 0; i < SHA_SUM_LENGTH; i++) {
                auto rnd_val = 0xff * engine() / (engine.max() - engine.min());
                sha_sum1[i] = rnd_val;
        }

        ASSERT_FALSE(checkShaSums(sha_sum1, sha_sum2));
}

TEST(storagetest, writeReadFileTest) {
        const char *test_file_path = "/tmp/storagetest.raw";

        FileInformation *file_info_write = fiInit();
        FileInformation *file_info_read = fiInit();

        Status *status = statusInit();

        fiSetPath((char *)test_file_path, file_info_write);
        // 10 MiB write
        fiSetFileSize(10 * 1024 * 1024, file_info_write);
        fiSetBlockSize(FILE_BLOCK_SIZE_DEFAULT, file_info_write);
        fiSetContentType(CONSTANT, file_info_write);
        fiSetContentConstant(0xa1, file_info_write);

        statusSetValue(status, BUSY);
        statusResetCurrentNumber(status);

        ThreadArg *targ;
        pthread_t tid_write = writeFile(file_info_write, status);
        while (statusGetValue(status) == BUSY) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                std::cout << "Write progress: " << statusGetProgress(status)
                          << " / " << fiGetFileSize(file_info_write)
                          << " bytes. " << statusGetCurrentWSpeed(status)
                          << " bytes/s." << std::endl;
        }
        pthread_join(tid_write, NULL);
        std::cout << "Average speed: " << statusGetAverageWSpeed(status)
                  << " bytes/s." << std::endl;
        std::cout << "Actual size: " << fiGetActualSize(file_info_write)
                  << " bytes." << std::endl;

        if (statusGetValue(status) == 2)
                std::cout << "device is full" << std::endl;

        if (statusGetValue(status) < 0)
                std::cout << "ERRNO: " << -statusGetValue(status) << std::endl;

        std::cout << "Sha sum: " << std::hex;

        const unsigned char *writed_sha_sum = fiGetShaSum(file_info_write);
        for (int i = 0; i < SHA_SUM_LENGTH; i++)
                std::cout << (unsigned int)(0xff & writed_sha_sum[i]);

        std::cout << std::dec << std::endl;

        // Read process
        statusResetCurrentNumber(status);
        fiSetFileSize(fiGetActualSize(file_info_write), file_info_read);
        fiSetBlockSize(FILE_BLOCK_SIZE_DEFAULT, file_info_read);

        fiSetPath((char *)test_file_path, file_info_read);

        statusSetValue(status, BUSY);
        pthread_t tid_read = readFile(file_info_read, status);
        while (statusGetValue(status) == BUSY) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                std::cout << "Read progress: " << statusGetProgress(status)
                          << " / " << fiGetFileSize(file_info_read)
                          << " bytes. " << statusGetCurrentRSpeed(status)
                          << " bytes/s." << std::endl;
        }
        pthread_join(tid_read, NULL);
        std::cout << "Actual size: " << fiGetActualSize(file_info_read)
                  << " bytes." << std::endl;

        std::cout << "Sha sum: " << std::hex;
        const unsigned char *readed_sha_sum = fiGetShaSum(file_info_read);
        for (int i = 0; i < SHA_SUM_LENGTH; i++)
                std::cout << (unsigned int)(0xff & readed_sha_sum[i]);

        std::cout << std::dec << std::endl;
        ASSERT_TRUE(checkShaSums((unsigned char *)writed_sha_sum,
                                 (unsigned char *)readed_sha_sum));
}

TEST(storagetest, UUIDFileNames) {
        auto uuid_name1 = fileNameGenerator();
        auto uuid_name2 = fileNameGenerator();
        std::cout << "UUID file names: " << std::endl
                  << uuid_name1 << std::endl
                  << uuid_name2 << std::endl;
        ASSERT_STRNE(uuid_name1, uuid_name2);
}

int main(int argc, char *argv[]) {
        ::testing::InitGoogleTest(&argc, argv);
        ::testing::InitGoogleMock(&argc, argv);

        return RUN_ALL_TESTS();
}
