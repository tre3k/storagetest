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
        return 0;
}
int testInDevice(char *path, int file_size, int block_size, Status *status) {
        return 0;
}
