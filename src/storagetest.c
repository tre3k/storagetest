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

static void *_threadReadFile(void *arg);
static void *_threadWriteFile(void *arg);

static pthread_t _createThread(FileInformation *file_info,
                               Status *status,
                               void *(*start_runtine)(void *arg));

static void *_threadReadFiles(void *arg);
static void *_threadWriteFiles(void *arg);

static pthread_t _createThreads(FileInformation **file_info,
                                Status *status,
                                void *(*start_runtine)(void *arg));

static void *_threadWriteFile(void *arg) {
        int i, j;
        clock_t astart, astop, cstop;
        double average_bytes_per_sec;
        double current_bytes_per_sec;

        ThreadArg *targ = arg;
        statusSetValue(targ->status, BUSY);

        const char *filepath = fiGetPath(targ->file_info);
        size_t file_size = fiGetFileSize(targ->file_info);
        size_t block_size = fiGetBlockSize(targ->file_info);
        enum ContentType type = fiGetContentType(targ->file_info);

        statusSetFilePath(targ->status, filepath);
        statusIncrementCurrentNumber(targ->status);

        unsigned char *buff = malloc(sizeof(unsigned char) * block_size);
        if (type == CONSTANT) {
                for (i = 0; i < block_size; i++)
                        buff[i] = fiGetContentConstant(targ->file_info);
        }

        if (type == RANDOM) srand(time(0));

        size_t current_size = 0;
        ssize_t writed;
        long int segments = file_size / block_size;
        size_t last_segment = file_size % block_size;

        EVP_MD_CTX *mdctx = EVP_MD_CTX_create();
        const EVP_MD *md = EVP_SHA();
        unsigned char *sha_sum = malloc(SHA_SUM_LENGTH);
        unsigned int sha_size;
        EVP_DigestInit_ex(mdctx, md, NULL);

        astart = clock();
        int file =
            open(filepath, O_CREAT | O_WRONLY, S_IWUSR | S_IRUSR | S_IRGRP);

        for (i = 0; i < segments + 1; i++) {
                if (i == segments) {
                        block_size = last_segment;
                        if (block_size == 0) break;
                }
                if (type == RANDOM) {
                        for (j = 0; j < block_size; j++) {
                                buff[j] = rand() % 255;
                        }
                }

                writed = write(file, buff, block_size);
                cstop = clock();

                if (writed < 0) break;
                current_size += writed;

                current_bytes_per_sec =
                    (double)current_size * CLOCKS_PER_SEC / (cstop - astart);
                statusSetCurrentWSpeed(targ->status, current_bytes_per_sec);
                statusSetProgress(targ->status, current_size);
                EVP_DigestUpdate(mdctx, buff, writed);
        }

        close(file);
        astop = clock();
        average_bytes_per_sec =
            (double)current_size * CLOCKS_PER_SEC / (astop - astart);
        statusSetAverageWSpeed(targ->status, average_bytes_per_sec);
        fiSetActualSize(targ->file_info, current_size);
        statusIncrProgressToAmounWrited(targ->status);
        EVP_DigestFinal_ex(mdctx, sha_sum, &sha_size);
        fiSetShaSum(sha_sum, targ->file_info);

        free(mdctx);
        free(sha_sum);

        switch (errno) {
                case 0:
                        statusSetValue(targ->status, DONE);
                        break;

                case ENOSPC:
                        statusSetValue(targ->status, DEVICE_IS_FULL);
                        break;

                default:
                        statusSetValue(targ->status, -errno);
                        break;
        }
        fiSetActualSize(targ->file_info, current_size);
        pthread_exit(0);
}

static void *_threadReadFile(void *arg) {
        clock_t astart, astop, cstop;
        double average_bytes_per_sec;
        double current_bytes_per_sec;

        ThreadArg *targ = arg;
        statusSetValue(targ->status, BUSY);

        const char *filepath = fiGetPath(targ->file_info);

        statusSetFilePath(targ->status, filepath);
        statusIncrementCurrentNumber(targ->status);

        int file = open(filepath, O_RDONLY);
        size_t block_size = fiGetBlockSize(targ->file_info);
        unsigned char *buff = malloc(sizeof(unsigned char) * block_size);

        EVP_MD_CTX *mdctx = EVP_MD_CTX_create();
        const EVP_MD *md = EVP_SHA();
        unsigned char *sha_sum = malloc(SHA_SUM_LENGTH);
        unsigned int sha_size;
        EVP_DigestInit_ex(mdctx, md, NULL);

        ssize_t current_read;
        ssize_t actual_size = 0;
        astart = clock();
        do {
                current_read = read(file, buff, block_size);
                cstop = clock();
                actual_size += current_read;
                current_bytes_per_sec =
                    (double)actual_size * CLOCKS_PER_SEC / (cstop - astart);
                statusSetCurrentRSpeed(targ->status, current_bytes_per_sec);
                statusSetProgress(targ->status, actual_size);

                EVP_DigestUpdate(mdctx, buff, current_read);
        } while (current_read > 0);
        astop = clock();
        average_bytes_per_sec =
            (double)actual_size * CLOCKS_PER_SEC / (astop - astart);
        statusSetAverageRSpeed(targ->status, average_bytes_per_sec);

        fiSetActualSize(targ->file_info, actual_size);
        statusIncrProgressToAmounRead(targ->status);
        EVP_DigestFinal_ex(mdctx, sha_sum, &sha_size);
        fiSetShaSum(sha_sum, targ->file_info);

        free(mdctx);
        free(sha_sum);

        switch (errno) {
                case 0:
                        statusSetValue(targ->status, DONE);
                        break;
                default:
                        statusSetValue(targ->status, -errno);
                        break;
        }
        pthread_exit(0);
}

static pthread_t _createThread(FileInformation *file_info,
                               Status *status,
                               void *(*start_runtine)(void *arg)) {
        pthread_t tid;
        pthread_attr_t thattr;
        pthread_attr_init(&thattr);

        ThreadArg *arg = malloc(sizeof(struct SThreadArg));
        arg->file_info = file_info;
        arg->status = status;

        pthread_create(&tid, &thattr, start_runtine, arg);
        return tid;
}

static void *_threadWriteFiles(void *arg) {
        ThreadsArg *targ = arg;
        FileInformation **fis = targ->file_infos;
        Status *status = targ->status;

        pthread_exit(0);
}

static pthread_t _createThreads(FileInformation **file_infos,
                                Status *status,
                                void *(*start_runtine)(void *arg)) {
        pthread_t tid;
        pthread_attr_t thattr;
        pthread_attr_init(&thattr);

        ThreadsArg *arg = malloc(sizeof(struct SThreadsArg));
        arg->file_infos = file_infos;
        arg->status = status;

        pthread_create(&tid, &thattr, start_runtine, arg);
        return tid;
}

pthread_t writeFile(FileInformation *file_info, Status *status) {
        return _createThread(file_info, status, _threadWriteFile);
}

pthread_t readFile(FileInformation *file_info, Status *status) {
        return _createThread(file_info, status, _threadReadFile);
}

bool checkShaSums(unsigned char *sha_sum1, unsigned char *sha_sum2) {
        unsigned int i;
        for (i = 0; i < SHA_SUM_LENGTH; i++)
                if (sha_sum1[i] != sha_sum2[i]) return false;

        return true;
}

pthread_t testInDirectory(char *path,
                          int files_count,
                          int file_size,
                          int block_size,
                          Status *status,
                          FileInformation **fis) {
        int i;
        pthread_t tid;
        fis = fiInitArray(files_count);
        char fullpath[PATH_MAX_LENGHT];

        /* filling structures of FileFnformation */
        for (i = 0; i < files_count; i++) {
                sprintf(fullpath, "%s/%s", path, fileNameGenerator());
        }

        tid = _createThreads(fis, status, _threadWriteFiles);
        return tid;
}

pthread_t testInDevice(char *path,
                       int file_size,
                       int block_size,
                       Status *status) {
        pthread_t tid;

        return tid;
}

const char *fileNameGenerator(void) {
        uuid_t binuuid;
        uuid_generate(binuuid);
        char *uuid = malloc(37);
        uuid_unparse(binuuid, uuid);
        return uuid;
}
