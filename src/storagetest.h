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

#ifndef _STORAGETEST_H_
#define _STORAGETEST_H_

#include <errno.h>
#include <fcntl.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "fileinformation.h"
#include "status.h"

typedef struct SThreadArg {
        FileInformation *file_info;
        Status *status;
} ThreadArg;

bool checkShaSums(unsigned char *sha_sum1, unsigned char *sha_sum2);

void *_threadWriteFile(void *arg);
void *threadReadFile(void *arg);

pthread_t writeFile(FileInformation *file_info, Status *status);
pthread_t readFile(FileInformation *file_info, Status *status);

pthread_t writeFiles(FileInformation **file_infos, int count, Status *status);
pthread_t readFiles(FileInformation **file_infos, int count, Status *status);
int removeFiles(FileInformation **file_infos, int count, Status *status);

int testInDirectory(
    char *path, int files_count, int file_size, int block_size, Status *status);
int testInDevice(char *path, int file_size, int block_size, Status *status);

#endif
