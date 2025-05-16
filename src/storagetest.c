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

#include "storagetest.h"

#include "fileinformation.h"

void *threadWriteFile(void *arg) {
        ThreadArg *targ = arg;
        const char *filepath = fiGetPath(targ->file_info);
        size_t file_size = fiGetFileSize(targ->file_info);
        size_t block_size = fiGetBlockSize(targ->file_info);

        unsigned char *buff = malloc(sizeof(unsigned char) * block_size);

        /* 1 - BUSY, 0 - DONE enum need create */
        statusSetValue(targ->status, 1);

        /* Here file write impl. need */
        for (int i = 0; i < 10; i++) {
                statusSetProgress(targ->status, i);
                sleep(1);
        }

        statusSetValue(targ->status, 0);
        pthread_exit(0);
}

pthread_t writeFile(FileInformation *file_info, Status *status) {
        pthread_t tid;
        pthread_attr_t thattr;
        pthread_attr_init(&thattr);

        ThreadArg *arg = malloc(sizeof(struct SThreadArg));
        arg->file_info = file_info;
        arg->status = status;

        pthread_create(&tid, &thattr, threadWriteFile, arg);

        return tid;
}

bool checkShaSums(unsigned char *sha_sum1, unsigned char *sha_sum2) {
        unsigned int i;
        for (i = 0; i < SHA_SUM_LENGTH; i++)
                if (sha_sum1[i] != sha_sum2[i]) return false;

        return true;
}

int testInDirectory(char *path,
                    int files_count,
                    int file_size,
                    int block_size,
                    Status *status) {
        printf("testInDirectory()\n");
        return 0;
}

int testInDevice(char *path, int file_size, int block_size, Status *status) {
        return 0;
}

int _mainStorageTest() {
        FileInformation *file_info = fiInit();
        Status *status = statusInit();

        fiSetPath("/tmp/test", file_info);
        fiSetFileSize(1024, file_info);
        fiSetBlockSize(32, file_info);

        ThreadArg *targ;
        pthread_t tid = writeFile(file_info, status);
        while (statusGetValue(status) != 0) {
                sleep(1);
                printf("Progress: %ld\n", statusGetProgress(status));
        }
        pthread_join(tid, NULL);

        return 0;
}
