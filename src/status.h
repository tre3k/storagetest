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

#ifndef _STATUS_H_
#define _STATUS_H_

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

struct SStatus;
typedef struct SStatus Status;

Status *statusInit();
void statusFree(Status *status);

char *statusGetFilePath(Status *status);
void statusSetFilePath(Status *status, const char *path);

void statusSetProgress(Status *status, size_t progress);
size_t statusGetProgress(Status *status);

void statusSetAverageWSpeed(Status *status, size_t average_write_speed);
size_t statusGetAverageWSpeed(Status *status);

void statusSetAverageRSpeed(Status *status, size_t average_read_speed);
size_t statusGetAverageRSpeed(Status *status);

void statusSetCurrentWSpeed(Status *status, size_t current_write_speed);
size_t statusGetCurrentWSpeed(Status *status);

void statusSetCurrentRSpeed(Status *status, size_t current_read_speed);
size_t statusGetCurrentRSpeed(Status *status);

void statusSetValue(Status *status, int value);
int statusGetValue(Status *status);

void statusResetCurrentNumber(Status *status);
int statusGetCurrentNumber(Status *status);
void statusIncrementCurrentNumber(Status *status);

#endif
