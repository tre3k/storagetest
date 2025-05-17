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

#include "fileinformation.h"

/*!
 * @path - path and name to file
 * @size - size of file
 * @block_size - size of block for read/written at a time
 * @sha_sum - sha256/512 check sum
 * @content_type - see ContentType
 * @content_constant - value of bytes if not random
 */
struct SFileInformation {
        char *path;
        size_t size;
        size_t block_size;
        size_t chunk;  // malloc(): unaligned tcache chunk detected ????
        unsigned char *sha_sum;
        unsigned int content_type;
        unsigned char content_constant;
};

size_t fiSize() { return sizeof(struct SFileInformation); }
FileInformation *fiInit() { return malloc(fiSize()); }
void fiFree(FileInformation *file_info) { free(file_info); }

const char *fiGetPath(FileInformation *file_info) { return file_info->path; }

void fiSetPath(char *path, FileInformation *file_info) {
        file_info->path = malloc(strnlen(path, PATH_MAX_LENGHT) + 1);
        strcpy(file_info->path, path);
}

const unsigned char *fiGetShaSum(FileInformation *file_info) {
        return file_info->sha_sum;
}

void fiSetShaSum(unsigned char *sha_sum, FileInformation *file_info) {
        file_info->sha_sum = malloc(SHA_SUM_LENGTH);
        int i;
        //! Very bad situation if sha_sum < SHA_SUM_LENGTH
        for (i = 0; i < SHA_SUM_LENGTH; i++) {
                file_info->sha_sum[i] = sha_sum[i];
        }
}

enum ContentType fiGetContentType(FileInformation *file_info) {
        return file_info->content_type;
}

void fiSetContentType(enum ContentType ct, FileInformation *file_info) {
        file_info->content_type = ct;
}

size_t fiGetFileSize(FileInformation *file_info) { return file_info->size; }
void fiSetFileSize(size_t size, FileInformation *file_info) {
        file_info->size = size;
}

size_t fiGetBlockSize(FileInformation *file_info) {
        return file_info->block_size;
}
void fiSetBlockSize(size_t size, FileInformation *file_info) {
        file_info->block_size = size;
}

unsigned char fiGetContentConstant(FileInformation *file_info) {
        return file_info->content_constant;
}

void fiSetContentConstant(unsigned char byte, FileInformation *file_info) {
        file_info->content_constant = byte;
}

FileInformation **fiInitArray(int count) {
        FileInformation **retval = malloc(count * fiSize());
        return retval;
}

void fiFreeArray(FileInformation **fi_array) { free(fi_array); }

FileInformation *fiGetFromArray(FileInformation **fi_array, int element) {
        return (FileInformation *)(fi_array + element * fiSize());
}
