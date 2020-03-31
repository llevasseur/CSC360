#ifndef FILE_H
#define FILE_H

#include <stddef.h>

/**
 * Write data to disk
 *
 * \param disk     The disk we want to write to
 * \param blockNum The block number
 * \param data     The data the is to be written
 */
 
void writeBlcok(FILE* disk, int blockNum, char* data);


/**
 * Initialize the vdisk for the LLFS
 *
 */
void InitLLFS();

#endif