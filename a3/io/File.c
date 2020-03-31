#define vdisk_path "../disk/vdisk"
#define BLOCK_SIZE 512
#define NUM_BLOCKS 4096
#define NUM_INODES 128 //This number is up to you
#define FREE_BLOCK 1 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "File.h"



struct inode {
	unsigned int i_size;
	unsigned int i_flag;
	unsigned char i_block_numbers[20];
	unsigned char i_sing_ind[2];
	unsigned char i_doub_ind[2];
}; //32 bytes

void readBlock(FILE* disk, int blockNum, char* buffer)
{
	fseek(disk, blockNum * BLOCK_SIZE, SEEK_SET);
	fread(buffer, BLOCK_SIZE, 1, disk);
}

void writeBlock(FILE* disk, int blockNum, char* data)
{
	fseek(disk, blockNum * BLOCK_SIZE, SEEK_SET);
	fwrite(data, BLOCK_SIZE, 1, disk);
}

void init_buffer(char* buffer, int size)
{
    for (int i = 0; i < size; i++) {
        buffer[i] = 0x0;
    }
}

void set_block(char* buffer, int block_num)
{
    int index = block_num / 8; //byte index
    int bit_index = block_num % 8; //bit index
    
    buffer[index] |= 1UL << bit_index;
}

int firstFreeBlock(FILE* disk)
{
	//return block number
	//go to block map
	//store block in a buffer
	//traverse until blockNum is found <- byte index and bit index
	//fill byte
	char* buffer;
	buffer = (char *) malloc(BLOCK_SIZE);
	readBlock(disk, FREE_BLOCK, buffer);
	int i = 0;
	int block = 0;
	bool blockFound = false;
	
	while (i <= BLOCK_SIZE && blockFound == false)
	{
		unsigned int value = buffer[i];
		printf("%d\n", value);
		
		char mask[32] = {0};
		for (int j = 0; j < 32; j++)
		{
			mask[j] = (value << j) & (1 << (8 * sizeof(unsigned int)- 1))
						? '1' : '0';
			if (mask[j] == '1') printf("Hit a 1! : %d  j = %d ", mask[j], j);
		}
		printf("\n");
		
		unsigned int reverse[32] = {0};
		for (int j = 0; j < 32; j++)
		{
			reverse[31 - j] = mask[j];
		}
		
		for (int j = 0; j < 32; j++)
		{
			if(reverse[j] == '1')
			{
				blockFound = true;
				block = i * 8 + j;
				reverse[j] = '0';
				break;
			}
		}
		
		if (blockFound)
		{
			unsigned int newValue[32] = {0};
			for (int j = 0; j < 32; j++)
			{
				newValue[31 - j] = reverse[j];
			}
			
			unsigned int val = 0;
			for (int j = 0; j < 32; j++)
			{
				//val << 1;
				val += newValue[j] - '0';
			}
			
			buffer[i] = val;
		}
		i++;
	}
	
	if (blockFound == false)
	{
		return -1;
	}
	
	writeBlock(disk, FREE_BLOCK, buffer);
	printf("block: %d\n", block);
	fclose(disk);
	return block;
}


void InitSuperblock()
{
	FILE* disk = fopen(vdisk_path, "rb+");
	
	//Intitialize Block 0 (Superblock)
	char* buffer;
	buffer = (char *) malloc(BLOCK_SIZE);
	int magic = 18;
	int blocks = NUM_BLOCKS; //4096
	int inodes = NUM_INODES; //128 <-may change
	memset(buffer, 0, BLOCK_SIZE);
	memcpy(buffer, &magic, sizeof(magic));
	memcpy(buffer + sizeof(int) * 1, &blocks, sizeof(int));
	memcpy(buffer + sizeof(int) * 2, &inodes, sizeof(int));
	writeBlock(disk, 0, buffer);
	free(buffer);
	fclose(disk);
}


void InitFreeBlockVector()
{
	FILE* disk = fopen(vdisk_path, "rb+");
	int i = 0;
	char* FreeBlockBuffer;
	FreeBlockBuffer = (char *) malloc(BLOCK_SIZE);
	
	
	//Initialize Block 1 (Free Block Vector)
	init_buffer(FreeBlockBuffer, 512); //Set all to 0
	
	//Initialize the first 10 (0 through 9) blocks as UNAVAILABLE (0)
	//So finish the remaining byte with 1's
	for(i = 8; i < 14; i++)
	{
		set_block(FreeBlockBuffer, i);
	}
	
	//Initialize the remaining blocks as AVAILABLE (1);
	for(i = 16; i < 4096; i++)
	{
		set_block(FreeBlockBuffer, i);
	}
	
	writeBlock(disk, 1, FreeBlockBuffer);
	fclose(disk);
}


void InitInodeBlockVector()
{
	FILE* disk = fopen(vdisk_path, "rb+");
	int i = 0;
	char* InodeBlockBuffer;
	InodeBlockBuffer = (char *) malloc(BLOCK_SIZE);
	
	char* InodeReadBlockBuffer;
	InodeReadBlockBuffer = (char *) malloc(BLOCK_SIZE);
	
	//Initialize the first free block (INODE MAP)
	init_buffer(InodeBlockBuffer, 512); //Set all to 0
	
	//Initialize the blocks as AVAILABLE
	for(i = 0; i < 4096; i++)
	{
		set_block(InodeBlockBuffer, i);
	}
	
	writeBlock(disk, 10, InodeBlockBuffer);
	readBlock(disk, 10, InodeReadBlockBuffer);
	printf("%u\n", InodeReadBlockBuffer[0]);
	
	firstFreeBlock(disk);
	
	
	// Set inode block to unavailable in block node
	//closeBlock();
	fclose(disk);
	
}


void InitLLFS()
{
	FILE* disk = fopen(vdisk_path, "ab"); //ab creates file if none exists
	char* init = calloc(BLOCK_SIZE * NUM_BLOCKS, 1); //inits to zero
	fwrite(init, BLOCK_SIZE * NUM_BLOCKS, 1, disk);
	free(init);
	fclose(disk);
	
	InitSuperblock();
	InitFreeBlockVector();
	InitInodeBlockVector();
	
	//initialize root directory
}

/**
 * Creating a file
 * 
 * TO DO:
 * 		Check to see if there are any free inode blocks
 *  	If there are then create new inode with name, type of file
 *  		(directory or regular) etc.
 *  	Check if there are any free data blocks and if so then allocate
 *  		one for the new file
 *  	Create a new directory entry for the new file in the given
 *  		directory location
 *  	If any of the can't be done then return an error
 */
 void createDirectory()
 {
	 //check free inode blocks
 }

/**
 * Deleting a file
 * 
 * TO DO:
 *  	Find the inode for that file given its directory listing and name
 *  	Get the location of the files blocks from the inode
 *  	Check if the file is regular file or directory -- if it is directory
 *  		check if it's empty or not
 *  	Deallocate the blocks listed in the inode -- set them as free in
 *  		block vector
 *  	Remove file listing from directory
 *  	Deallocate inode and set its location as free
 */

//Writing to a file


//Reading from a file - load a file from disk into memory


//Finding an inode -- reading -- modifying -- deleting etc.


//Finding an available block


//Formatting the disk -- initial set up and layout


//Checking the integrity of the inodes and the blocks

int main()
{
	InitLLFS();
    return 0;
}