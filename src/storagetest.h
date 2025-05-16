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
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stddef.h>
#include <stdlib.h>

#include "config.h"
#include "fileinformation.h"

bool checkShaSums(unsigned char *sha_sum1, unsigned char *sha_sum2);

int writeFile(FileInformation *file_info);
int readFile(FileInformation *file_info);

int writeFiles(FileInformation *file_infos[], int count);
int readFiles(FileInformation *file_infos[], int count);

#endif
