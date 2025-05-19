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

#ifndef _FILEINFORMATION_H_
#define _FILEINFORMATION_H_

#include <openssl/sha.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

enum ContentType {
        RANDOM,
        CONSTANT
};

struct SFileInformation;
typedef struct SFileInformation FileInformation;

FileInformation *fiInit();
void fiFree(FileInformation *file_info);
size_t fiSize(void);

const char *fiGetPath(FileInformation *file_info);
void fiSetPath(FileInformation *file_info, char *path);

const unsigned char *fiGetShaSum(FileInformation *file_info);
void fiSetShaSum(FileInformation *file_info, unsigned char *sha_sum);

enum ContentType fiGetContentType(FileInformation *file_info);
void fiSetContentType(FileInformation *file_info, enum ContentType ct);

size_t fiGetFileSize(FileInformation *file_info);
void fiSetFileSize(FileInformation *file_info, size_t size);

size_t fiGetBlockSize(FileInformation *file_info);
void fiSetBlockSize(FileInformation *file_info, size_t size);

unsigned char fiGetContentConstant(FileInformation *file_info);
void fiSetContentConstant(FileInformation *file_info, unsigned char byte);

FileInformation **fiInitArray(int count);
void fiFreeArray(FileInformation **file_infos);
FileInformation *fiGetFromArray(FileInformation **fi_array, int element);

size_t fiGetActualSize(FileInformation *file_info);
void fiSetActualSize(FileInformation *file_info, size_t size);

#endif
