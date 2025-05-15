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

enum ContentType {
        RANDOM,
        CONSTANT
};

/*!
 * @path - path and name to file
 * @size - size of file
 * @block_size - size of block for read/written at a time
 * @progress - bytes of current writed/readed
 * @sha_sum - sha256/512 check sum
 * @content_type - see ContentType
 * @content_constant - value of bytes if not random
 */

struct SFileInformation {
        char *path;
        size_t size;
        size_t block_size;
        size_t progress;
        unsigned char *sha_sum;
        unsigned int content_type;
        unsigned char content_constant;
};

size_t fileInformationSize() { return sizeof(FileInformation); }
FileInformation *fileInformationInit() { return malloc(fileInformationSize()); }
void fileInformationFree(FileInformation *file_info) { free(file_info); }
