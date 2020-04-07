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

//llevasseur@linux.csc.uvic.ca

int inodeMapBlock;


struct inode {
	unsigned int i_size;
	unsigned int i_flag;
	unsigned char i_block_numbers[10];
	unsigned char i_sing_ind;
	unsigned char i_doub_ind;
}; //32 bytes

void readBlock(FILE* disk, int blockNum, char* buffer)
{
	fseek(disk, blockNum * BLOCK_SIZE, SEEK_SET);
	fread(buffer, BLOCK_SIZE, 1, disk);
	printf("\nHERE IN readBLOCK: buffer[0] = %u\n", buffer[0]);
}

void writeBlock(FILE* disk, int blockNum, char* data)
{
	char* tmpbuffer;
	tmpbuffer = (char *) malloc(BLOCK_SIZE);
	fseek(disk, blockNum * BLOCK_SIZE, SEEK_SET);
	fwrite(data, BLOCK_SIZE, 1, disk);
	printf("\nIN writeBlock ATM: data[0] = %u   GO TO READBLOCK\n", data[0]);
	readBlock(disk, blockNum, tmpbuffer);
	free(tmpbuffer);
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


int firstFreeBlock(FILE* disk, int block_num)
{
	printf("Block num: %d\n", block_num);
	char* buffer;
	buffer = (char *) malloc(BLOCK_SIZE);
	readBlock(disk, block_num, buffer);
	//printf("%u\n", buffer[0]);
	
	int block;
	int j = 0;

	bool blockFound = false;
	
	while (j <= BLOCK_SIZE && blockFound == false)
	{
		unsigned int value = buffer[j];
		printf("\nStarting FREEBLOCK While loop:\n");
		printf("Buffer[%d]: %u\n", j, value);
		for(int i = 7; i >= 0; i--)
		{
			unsigned int byte = value >> i;
			printf("byte %d: %u\n", i, byte);
			if (byte == 1)
			{
				blockFound = true;
				block = (8 * j) + abs(i - 7);
				printf("Your block is %d\n\n", block);
				buffer[j] = buffer[j] >> 1;
				//printf("buffer[%d]: %u\n", j, buffer[j]);
				break;
			}
		}
		j++;
	}
	
	if (blockFound == false)
	{
		return -1;
	}
	
	writeBlock(disk, block_num, buffer);
	free(buffer);
	fclose(disk);
	return block;
	
}



int firstFreeInodeBlock(FILE* disk, int block_num)
{
	printf("Block num: %d\n", block_num);
	char* buffer;
	buffer = (char *) malloc(BLOCK_SIZE);
	readBlock(disk, block_num, buffer);
	//printf("%u\n", buffer[0]);
	
	int block;
	int j = 0;

	bool blockFound = false;
	
	while (j <= BLOCK_SIZE && blockFound == false)
	{
		unsigned char value = buffer[j];
		//printf("\nStarting While loop:\n");
		//printf("Buffer[%d]: %u\n", j, value);
		for(int i = 31; i >= 0; i--)
		{
			unsigned char byte = value >> i;
			//printf("byte %d: %u\n", i, byte);
			if (byte == 1)
			{
				blockFound = true;
				block = (32 * j) + abs(i - 31);
				//printf("Your next inode is: %d\n", block);
				//printf("PREBLOCKINFO: buffer[0] = %u\n", buffer[j]);
				buffer[j] = buffer[j] >> 1;
				//printf("bitshifted buffer[%d]: %u\n", j, buffer[j]);
				break;
			}
		}
		j++;
	}
	
	if (blockFound == false)
	{
		return -1;
	}
	
	writeBlock(disk, block_num, buffer);
	free(buffer);
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
	free(FreeBlockBuffer);
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
	
	printf("----------ENTERING firstFreeBlock to find free Block---------\n");
	
	
	inodeMapBlock = firstFreeBlock(disk, FREE_BLOCK);
	
	
	printf("----RETURN with inodeMapBlock = %d  Check contents of block:--------\n", inodeMapBlock);
	printf("***ABOUT TO WRITE TO BLOCK***\ncontents of InodeBlockBuffer[0] = %u\n", InodeBlockBuffer[0]);
	
	writeBlock(disk, inodeMapBlock, InodeBlockBuffer);
	
	printf("\tOPEN NEW DISK NAME\n");
	
	
	readBlock(disk, inodeMapBlock, InodeReadBlockBuffer);
	
	
	printf("!!!!What is inodeMapBlock = %d!!!!\n", inodeMapBlock);
	printf("First Four bytes of InodeMapBlock: %u\n", InodeReadBlockBuffer[0]);
	
	
	readBlock(disk, 10, InodeReadBlockBuffer);
	
	
	printf("???What is First Four bytes of 10: %u????\n", InodeReadBlockBuffer[0]);
	printf("FOUND PROBLEM! IT'S THE readBlock FUNCTION\n");
	
	
	
	
	free(InodeBlockBuffer);
	free(InodeReadBlockBuffer);
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
 void createDirectory(char* filename, unsigned int flag)
 {
	if (flag > 1 || flag < 0)
	{
		printf("error: flag size incorrect for new file.\n");
		exit(0);
	}
	 
	FILE* disk = fopen(vdisk_path, "rb+");
	 
	struct inode cur_inode;
	 
	cur_inode.i_size = 0;
	cur_inode.i_flag = flag;
	 
	cur_inode.i_block_numbers[0] = firstFreeBlock(disk, FREE_BLOCK);
	for(int i = 1; i < 10; i++)
	{
		cur_inode.i_block_numbers[i] = 0;
	}
	cur_inode.i_sing_ind = 0;
	cur_inode.i_doub_ind = 0;
	 
	int freeInode;
	printf("InodeMapBlock: %d\n", inodeMapBlock);
	fclose(disk);
	
	
	FILE* disk1 = fopen(vdisk_path, "rb+");
	
	char* buffer;
	buffer = (char *) malloc(BLOCK_SIZE);
	readBlock(disk1, inodeMapBlock, buffer);
	//printf("YOU ARE HERE%u\n", buffer[0]);
	
	
	freeInode = firstFreeInodeBlock(disk, inodeMapBlock) + 1;
	printf("Free inode: %d\n", freeInode);

		 	 
	 //create inode structure
	 
	 //check free inode blocks
	 fclose(disk1);
	 free(buffer);
	 
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
	createDirectory("root", 0);
    return 0;
}