#ifndef DISK_H
#define DISK_H

#include <stddef.h>

void readBlock(FILE* disk, int blockNum, char* buffer);

void writeBlock(FILE* disk, int blockNum, char* data);

#endif
