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

#include "status.h"

struct SStatus {
        char *current_path_file;
        int current_number_file;
        size_t progress;             // in bytes
        size_t average_write_speed;  // in bytes/sec.
        size_t average_read_speed;
        size_t current_write_speed;
        size_t current_read_speed;
};

Status *statusInit() { return malloc(sizeof(struct SStatus)); }
void statusFree(Status *status) { free(status); }

char *statusGetCurrentFile(Status *status);
int *statusGetCurrentNumberFile(Status *status);
