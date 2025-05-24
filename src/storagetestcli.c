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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "fileinformation.h"
#include "status.h"
#include "storagetest.h"

static const char *human_size(uint64_t bytes_to_convert) {
        char *suffix[] = {"B", "KiB", "MiB", "GiB", "TiB"};
        char length = sizeof(suffix) / sizeof(suffix[0]);

        int i = 0;
        double dblBytes = bytes_to_convert;

        if (bytes_to_convert > 1024) {
                for (i = 0; (bytes_to_convert / 1024) > 0 && i < length - 1;
                     i++, bytes_to_convert /= 1024)
                        dblBytes = bytes_to_convert / 1024.0;
        }

        static char output[200];
        sprintf(output, "%.02lf %s", dblBytes, suffix[i]);
        return output;
}

int main(int argc, char *argv[]) {
        const char *path = argv[1];
        if (path == NULL) {
                printf("Noting.\n");
                return 0;
        }
        printf("Test path: %s\n", path);

        int i, update_time = 1;

        // Write
        int f_count = 1024;               // Amout 1 TiB
        int f_size = 1024 * 1024 * 1024;  // 1 GiB

        Status *status = statusInit();
        FileInformation **fis_write = fiInitArray(f_count);

        pthread_t tid = writeFilesToDirectory((char *)path, f_count, f_size,
                                              FILE_BLOCK_SIZE_DEFAULT, RANDOM,
                                              0xea, status, fis_write);
        char format_speed[200];
        char format_avr_speed[200];
        char format_size[200];
        char format_full_size[200];

        sprintf(format_full_size, "%s", human_size(f_size));

        do {
                sleep(update_time);

                sprintf(format_speed, "%s/s.",
                        human_size(statusGetCurrentWSpeed(status)));
                sprintf(format_avr_speed, "%s/s.",
                        human_size(statusGetAverageWSpeed(status)));
                sprintf(format_size, "%s",
                        human_size(statusGetProgress(status)));

                printf(
                    "\r  Write #%d: size: %s/%s, speed: %s, avr. speed: %s"
                    "                                   \r",
                    statusGetCurrentNumber(status), format_size,
                    format_full_size, format_speed, format_avr_speed);
                fflush(stdout);
        } while (statusGetValue(status) == BUSY);
        printf("\n");
        pthread_join(tid, NULL);

        if (statusGetValue(status) == DEVICE_IS_FULL) {
                printf("There is no free space left on the device\n");
        }

        printf("%d files writed\n", statusGetCurrentNumber(status));
        printf("Full size writed: %lu bytes, (%s).\n",
               statusGetAmountWrited(status),
               human_size(statusGetAmountWrited(status)));

        // Read
        int count_for_read = statusGetCurrentNumber(status);
        statusResetCurrentNumber(status);
        int current_number;

        FileInformation *fi_read;
        FileInformation *fi_write;
        FileInformation **fis_read = fiInitArray(count_for_read);
        size_t *file_sizes = malloc(sizeof(size_t) * count_for_read);

        for (int i = 0; i < count_for_read; i++) {
                fi_read = fiGetFromArray(fis_read, i);
                fi_write = fiGetFromArray(fis_write, i);
                fiSetPath(fi_read, (char *)fiGetPath(fi_write));
                file_sizes[i] = fiGetActualSize(fi_write);
                fiSetFileSize(fi_read, file_sizes[i]);
                fiSetBlockSize(fi_read, FILE_BLOCK_SIZE_DEFAULT);
        }

        tid = readFiles(fis_read, count_for_read, status);
        do {
                sleep(update_time);
                current_number = statusGetCurrentNumber(status);

                sprintf(format_speed, "%s/s.",
                        human_size(statusGetCurrentRSpeed(status)));
                sprintf(format_size, "%s",
                        human_size(statusGetProgress(status)));
                if (current_number < count_for_read) {
                        sprintf(format_full_size, "%s",
                                human_size(file_sizes[current_number]));
                }
                printf(
                    "\r  Read #%d: size: %s/%s, speed: %s"
                    "                                   \r",
                    statusGetCurrentNumber(status), format_size,
                    format_full_size, format_speed);
                fflush(stdout);
        } while (statusGetValue(status) == BUSY);
        printf("\n");
        pthread_join(tid, NULL);

        printf("Full size read: %lu bytes, (%s). \n",
               statusGetAmountWrited(status),
               human_size(statusGetAmountRead(status)));

        bool all_ok = true;
        size_t fault_size = 0;
        for (i = 0; i < count_for_read; i++) {
                fi_read = fiGetFromArray(fis_read, i);
                fi_write = fiGetFromArray(fis_write, i);

                if (!checkShaSums((unsigned char *)fiGetShaSum(fi_read),
                                  (unsigned char *)fiGetShaSum(fi_write))) {
                        all_ok = false;
                        printf("Sha test fault for #%d: %s\n", i,
                               fiGetPath(fi_read));
                        fault_size += fiGetActualSize(fi_read);
                }
        }

        printf("Write speed: %s/s.\n",
               human_size(statusGetAverageWSpeed(status)));
        printf("Read speed: %s/s.\n",
               human_size(statusGetAverageRSpeed(status)));

        size_t actual_size = statusGetAmountWrited(status) - fault_size;
        printf("Actual size about: %lu bytes, (%s)\n", actual_size,
               human_size(actual_size));

        if (!all_ok) {
                printf("With error!\n");
        }

        return 0;
}
