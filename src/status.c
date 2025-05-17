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

/*!
 * @current_path_file
 * @current_number_file
 * @progress - read/writed current bytes
 * @average_write_speed - wirte speed for full file
 * @average_read_speed -  read speed for full file
 * @current_write_speed - write speed for block
 * @current_read_speed - read speed for block
 * @value - value for some status
 */
struct SStatus {
        char current_path_file[PATH_MAX_LENGHT];
        unsigned int current_number_file;
        size_t progress;             // in bytes
        double average_write_speed;  // in bytes/sec.
        double average_read_speed;
        double current_write_speed;
        double current_read_speed;
        int value;
};

Status *statusInit() { return malloc(sizeof(struct SStatus)); }
void statusFree(Status *status) { free(status); }

char *statusGetFilePath(Status *status) { return status->current_path_file; }
void statusSetFilePath(Status *status, const char *path) {
        /*!
         * Защита от переполнения буфера, но возможна и другая
         * реализация, например все же копировать часть строки,
         * которая влазит
         */
        if (strlen(path) > PATH_MAX_LENGHT) return;
        strcpy(status->current_path_file, path);
}

void statusSetProgress(Status *status, size_t progress) {
        status->progress = progress;
}
size_t statusGetProgress(Status *status) { return status->progress; }

void statusSetAverageWSpeed(Status *status, double average_write_speed) {
        status->average_write_speed = average_write_speed;
}

double statusGetAverageWSpeed(Status *status) {
        return status->average_write_speed;
}

void statusSetAverageRSpeed(Status *status, double average_read_speed) {
        status->average_read_speed = average_read_speed;
}

double statusGetAverageRSpeed(Status *status) {
        return status->average_read_speed;
}

void statusSetCurrentWSpeed(Status *status, double current_write_speed) {
        status->current_write_speed = current_write_speed;
}

double statusGetCurrentWSpeed(Status *status) {
        return status->current_write_speed;
}

void statusSetCurrentRSpeed(Status *status, double current_read_speed) {
        status->current_read_speed = current_read_speed;
}
double statusGetCurrentRSpeed(Status *status) {
        return status->current_read_speed;
}

void statusSetValue(Status *status, int value) { status->value = value; }
int statusGetValue(Status *status) { return status->value; }

void statusResetCurrentNumber(Status *status) {
        status->current_number_file = 0;
}

int statusGetCurrentNumber(Status *status) {
        return status->current_number_file;
}

void statusIncrementCurrentNumber(Status *status) {
        status->current_number_file++;
}
