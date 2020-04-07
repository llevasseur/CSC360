//By Leevon Levasseur, v00868326, April 6, 2020

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include "File.h"

#define vdisk_path "../disk/vdisk"
#define BLOCK_SIZE 512
#define NUM_BLOCKS 4096
#define NUM_INODES 128 //This number is up to you
#define INODE_START_BLOCK 2 // Blocks 2 - 9 used for inode data
#define FREE_BLOCK 1 //free block bit map
#define ENTRY_SIZE 32


typedef enum
{
	DIRECTORY = 0,
	FLAT = 1,
} file_type;

struct inode
{
	int num;
	int fileSize;
	int type;
	unsigned char blocks[20];
};


//GLOBAL VARIABLES
int curDirectoryBlock;

int inode = 1;

int curInodeNum = 1;

int curDirectoryInode = 1;

file_type curType = DIRECTORY;

int cursor = 0;

//END

int myPow(int a, int b)
{
	int ans = a;
	int i = 1;
	while (1 < b)
	{
		ans = ans * a;
		i++;
	}
	return ans;
}


/******
	readBlock() and writeBlock() given from the tutorial
	*******/
	
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


/******
	Check the freeBlock Vector and find the first free bit (1)
	
	If a free bit is found, the block number is found, the freeBlock vector is updated to switch that bit to 0
	
	Return the block number if found,
	
	Else return -1
	*******/
	
int firstFreeBlock()
{
	FILE* disk = fopen(vdisk_path, "rb+");
	unsigned char* buffer;
	buffer = (unsigned char*) malloc(BLOCK_SIZE);
	readBlock(disk, FREE_BLOCK, (char*)buffer);
	
	int i = 0;
	
	int block = 0;
	bool blockFound = false;
	
	while (i <= BLOCK_SIZE && blockFound == false)
	{
		int value = (int)buffer[i];
		int j;
		
		for (j = 7; j >= 0; j--)
		{
			if ((int)(value / myPow(2, j)) > 0)
			{
				block = i * 8 + (7 - j);
				blockFound = true;
				break;
			}
		}
		
		if (blockFound)
		{
			value -= myPow(2, j);
			memcpy(buffer + i, &value, 1);
			writeBlock(disk, FREE_BLOCK, (char*)buffer);
			break;
		} else i++;
	}
	
	if (blockFound == false)
	{
		fclose(disk);
		free(buffer);
		return -1;
	}
	
	char* resetBuffer;
	resetBuffer = (char*) malloc(BLOCK_SIZE);
	memset(resetBuffer, 0, BLOCK_SIZE);
	writeBlock(disk, block, resetBuffer);
	
	fclose(disk);
	free(resetBuffer);
	free(buffer);
	
	return block;
}


/******
	Find if a file exists in requested directory
	
	Read each line and compare that line with the requested name
	
	If found return the result's inode
	
	Else return -1 -> to be tested later
	*******/

int findFile(char* name, int blockNum)
{
	FILE* disk = fopen(vdisk_path, "rb+");
	unsigned char* buffer;
	buffer = (unsigned char*) malloc(BLOCK_SIZE);
	readBlock(disk, blockNum, (char*)buffer);
	
	for (int i = 0; i < BLOCK_SIZE; i += 32)
	{
		char findName[32] = {'\0'};
		
		for (int j = 1; j < 32; j++)
		{
			//Found byte after name
			if ((int)buffer[i + j] == 0)
			{
				findName[j] = '\0';
				break;
			}
			
			findName[j - 1] = (char)((int)buffer[i + j]);
		}
		
		//Current findName equals the given name, return inode (first byte of 32 entries)
		if (strcmp(name, findName) == 0)
		{
			int resultInode = (int)buffer[i];
			fclose(disk);
			free(buffer);
			return resultInode;
		}
	}
	
	//else return -1 (not found)
	fclose(disk);
	free(buffer);
	return -1;
}



/******
	Check if the requested directory has any possible space left
	
	If an empty entry exists in the directory, return true
	
	Else the directory has 16 entries already, return false
	*******/
	
bool checkDirectorySpace(int blockNum)
{
	FILE* disk = fopen(vdisk_path, "rb+");
	unsigned char* buffer;
	buffer = (unsigned char*) malloc(BLOCK_SIZE);
	readBlock(disk, blockNum, (char*)buffer);
	
	for (int i = 0; i < BLOCK_SIZE; i += 32)
	{
		if ((int)buffer[i] == 0)
		{
			fclose(disk);
			free(buffer);
			return true;
		}
	}
	
	fclose(disk);
	free(buffer);
	return false;
}


/******
	Make a directory file in the current directory
	
	Find first free block and store that number in the first free inode
	
	in current directory, store file's inode, name
	
	in new directorys block, give reference to the parent directory
	*******/
void makeDir(char* name)
{
	//Check current directory's space to make sure there is enough
	if (!checkDirectorySpace(curDirectoryBlock))
	{
		printf("This directory has no space for this file.\n");
		return;
	}
	
	//make sure a file or directory does not have this name already
	int checkName = findFile(name, curDirectoryBlock);
	
	if (checkName != -1)
	{
		printf("\"%s\" is the name of another file or folder in this directory.\n", name);
		return;
	}
	
	//wrtie the inode into the inode map blocks (metadata)
	FILE* disk = fopen(vdisk_path, "rb+");
	struct inode new_inode;
	new_inode.num = inode;
	new_inode.type = DIRECTORY;
	new_inode.blocks[0] = firstFreeBlock();
	
	int block = new_inode.blocks[0];
	int type = 0;
	int fileSize = 0;
	
	char* buffer;
	buffer = (char*) malloc(BLOCK_SIZE);
	readBlock(disk, inode/16 + INODE_START_BLOCK, buffer);
	
	int curInodeOffset = ((inode - 1) % 16) * 32;
	
	memcpy(buffer + curInodeOffset, &fileSize, 4);
	memcpy(buffer + 4 + curInodeOffset, &type, 4);
	memcpy(buffer + 8 + curInodeOffset, &block, 2);
	
	//initailize back half to 0's
	for (int i = 1; i < 10; i++)
	{
		int zero_pointer = 0;
		memcpy(buffer + 8 + curInodeOffset + (i * 2), &zero_pointer, 2);
	}
	
	writeBlock(disk, inode/16 + INODE_START_BLOCK, buffer);
	
	//truncate the name if needed
	char* directoryName;
	directoryName = (char*) malloc(strlen(name) + 1);
	
	int i;
	
	for (i = 0; i < strlen(name) && i < 30; i++)
		directoryName[i] = name[i];
	
	directoryName[i] = '\0';
	fclose(disk);
	
	//write to the current block
	disk = fopen(vdisk_path, "rb+");
	char* buffer2;
	buffer2 = (char*) malloc(BLOCK_SIZE);
	readBlock(disk, curDirectoryBlock, buffer2);
	
	int j;
	
	for (j = 0; j < BLOCK_SIZE; j += 32)
		if ((unsigned int)buffer2[j] == 0) break;

	memcpy(buffer2 + j, &inode, 1);
	
	for (int k = 0; k <= i; k++)
		memcpy(buffer2 + (k + 1) + j, &directoryName[k], 1);
	
	writeBlock(disk, curDirectoryBlock, buffer2);
	
	//add the parent directory to the new folder block
	char* buffer3;
	buffer3 = (char*) malloc(BLOCK_SIZE);
	char parent[2] = {'.', '.'};
	
	
	memset(buffer3, 0, BLOCK_SIZE);
	memcpy(buffer3, &curInodeNum, 1);
	memcpy(buffer3 + 1, &parent, sizeof(parent));
	writeBlock(disk, new_inode.blocks[0], buffer3);
	
	fclose(disk);
	free(buffer3);
	free(buffer2);
	free(directoryName);
	free(buffer);
	
	inode++;
}


/******
	Make a flat file in the current directory
	
	Find first free block and store that number in the first free inode
	
	in current directory, store file's inode, name
	
	in new flat file block, give reference to the parent directory
	*******/
	
void makeFlat(char* name)
{
	//Check current directory's space to make sure there is enough
	if(!checkDirectorySpace(curDirectoryBlock))
	{
		printf("The current directory has no space for this flat file.\n");
		return;
	}
	
	//make sure a file or directory does not have this name already
	int checkName = findFile(name, curDirectoryBlock);
	
	if (checkName != -1)
	{
		printf("\"%s\" is the name of another file in this directory.\n", name);
		return;
	}
	
	FILE* disk = fopen(vdisk_path, "rb+");
	struct inode new_inode;
	new_inode.num = inode;
	new_inode.type = FLAT;
	new_inode.blocks[0] = firstFreeBlock();
	
	for (int i = 1; i < 20; i++)
		new_inode.blocks[i] = 0;
	
	int block = new_inode.blocks[0];
	int type = 1;
	int fileSize = 0;
	
	char* buffer;
	buffer = (char*) malloc(BLOCK_SIZE);
	readBlock(disk, inode/16 + INODE_START_BLOCK, buffer);
	
	int curInodeOffset = ((inode - 1) % 16) * 32;
	
	memcpy(buffer + curInodeOffset, &fileSize, 4);
	memcpy(buffer + 4 + curInodeOffset, &type, 4);
	memcpy(buffer + 8 + curInodeOffset, &block, 2);
	
	for (int i = 1; i < 10; i++)
	{
		int zero_pointer = 0;
		memcpy(buffer + curInodeOffset + 8 + (i * 2), &zero_pointer, 2);
	}
	
	writeBlock(disk, inode/16 + INODE_START_BLOCK, buffer);
	
	char* writeBuffer;
	writeBuffer = (char*) malloc(BLOCK_SIZE);
	memset(writeBuffer, 0, BLOCK_SIZE);
	writeBlock(disk, new_inode.blocks[0], writeBuffer);
	
	char* fileName;
	fileName = (char*) malloc(strlen(name) + 1);
	
	int i;
	
	for (i = 0; i < strlen(name) && i < 30; i++)
		fileName[i] = name[i];
	
	fileName[i] = '\0';
	
	char* buffer2;
	buffer2 = (char*) malloc(BLOCK_SIZE);
	readBlock(disk, curDirectoryBlock, buffer2);
	
	int j;
	
	for(j = 0; j < BLOCK_SIZE; j += 32)
		if ((unsigned int)buffer2[j] == 0) break;

	memcpy(buffer2 + j, &inode, 1);
	
	for (int k = 0; k <= i; k++)
		memcpy(buffer2 + (k + 1) + j, &fileName[k], 1);
	
	writeBlock(disk, curDirectoryBlock, buffer2);
	
	
	fclose(disk);
	free(buffer2);
	free(fileName);
	free(buffer);
	
	inode++;
}

/******
	Open a file
	
	Check if no FLAT files are open and potentially ready to be written to
	
	Find the name in the current directory block
	
	Extract the correct data from the block
	
	Check to see if the file is a DIRECTORY or FLAT and adjust global variables
	
	Ready to use current file.
	
	PS: calling openFile(..) allows you to return to the parent directory
	*******/
	
void openFile(char* name)
{
	//Check to see if a FLAT file is already open and able to be written to
	if (curType == FLAT)
	{
		printf("hiya\n");
		printf("File currently open. It must be closed before continuing.\n");
		return;
	}
	
	int fileInode = findFile(name, curDirectoryBlock);
	
	//Check if a file with the given name truly exists
	if (fileInode == -1)
	{
		printf("File %s not found!\n", name);
		return;
	}
	
	int blockNum = fileInode/16 + INODE_START_BLOCK;
	
	FILE* disk = fopen(vdisk_path, "rb+");
	unsigned char* buffer;
	buffer = (unsigned char*) malloc(BLOCK_SIZE);
	readBlock(disk, blockNum, (char*)buffer);
	
	int type = (int)buffer[((fileInode - 1) % 16) * 32 + 4];
	unsigned int block = buffer[((fileInode - 1) % 16) * 32 + 8];
	
	if (type == 0)
	{
		//If request is to open DIRECTORY file, update current directory data and other global data
		curDirectoryBlock = block;
		curType = DIRECTORY;
		curInodeNum = fileInode;
		curDirectoryInode = fileInode;
		
	} else if (type == 1){
		
		//If request is to open FLAT file, find fileSize, update cursor and update global data
		unsigned char firstByte = buffer[((fileInode - 1) % 16) * 32 + 1];
		unsigned char secondByte = buffer[((fileInode - 1) % 16) * 32];
		
		int fileSize = (int)firstByte * 256 + (int)secondByte;
		cursor = fileSize;
		
		//Something has already been written, go to next location
		if (cursor != 0) cursor++;
		
		curType = FLAT;
		curInodeNum = fileInode;
		
	} else {
		//Catch all
		printf("Unknown type error! Neither DIRECTORY nor FLAT\n");
		return;
	}
	
	
	fclose(disk);
	free(buffer);
	printf("Opening %s %s\n", type == 0 ? "directory" : "file", name);
}

/******
	Close current FLAT file
	
	Will do nothing if a FLAT file is not open. i.e. if closeFile() called before openFile(name);
	OpenFile() makes sure 2 files are never open at the same time
	*******/
	
void closeFile()
{
	int inodeBlockNum = curInodeNum/16 + INODE_START_BLOCK;
	
	FILE* disk = fopen(vdisk_path, "rb+");
	unsigned char* buffer;
	buffer = (unsigned char*) malloc(BLOCK_SIZE);
	readBlock(disk, inodeBlockNum, (char*)buffer);
	
	//New size of the file is the size of cursor, update
	memcpy(buffer + ((curInodeNum - 1) % 16) * 32, &cursor, 4);
	writeBlock(disk, inodeBlockNum, (char*)buffer);
	
	//Reset cursor and return set inode to current directory inode
	cursor = 0;
	curType = DIRECTORY;
	curInodeNum = curDirectoryInode;
	
	fclose(disk);
	free(buffer);
}



/******
	Write text to a FLAT file
	
	supporting only one blocks per FLAT file, no more
	
	*******/
	
void writeToFile(char* text)
{
	//Make sure file is a FLAT file
	if (curType == DIRECTORY)
	{
		printf("Can't write to a folder!\n");
		return;
	}
	
	int inodeBlockNum = curInodeNum/16 + INODE_START_BLOCK;
	
	FILE* disk = fopen(vdisk_path, "rb+");
	unsigned char* buffer;
	buffer = (unsigned char*) malloc(BLOCK_SIZE);
	readBlock(disk, inodeBlockNum, (char*)buffer);
	
	//the block start at byte 8, and every 512 bytes in fileSize is another block
	
	int curInodeOffset = ((curInodeNum - 1) % 16) * 32;
	unsigned char firstByte = buffer[curInodeOffset + 9 + ((cursor/512) * 2)];
	unsigned char secondByte = buffer[curInodeOffset + 8 + ((cursor/512) * 2)];
	
	int block = (int)firstByte * 256 + (int)secondByte;
	
	int newFileSize = cursor + strlen(text);
	int newBlock = -1;
	
	//supporting only writing one block per flat file
	if (newFileSize > (cursor/512 + 1) * BLOCK_SIZE)
	{
		printf("This file doesn't have enough space for this text.\n");
		return;
	}
	
	fclose(disk);
	
	
	disk = fopen(vdisk_path, "rb+");
	char* writeBuffer;
	writeBuffer = (char*) malloc(BLOCK_SIZE);
	readBlock(disk, block, writeBuffer);
	
	
	int i;
	
	//copy text into writeBuffer
	for(i = 1; i <= strlen(text); i++)
	{
		if (cursor < BLOCK_SIZE)
		{
			memcpy(writeBuffer + (cursor % 512), &text[i - 1], 1);
			cursor++;
		} else break;
	}
	
	writeBlock(disk, (int)block, writeBuffer);
	
	writeBlock(disk, inodeBlockNum, (char*)buffer);
	
	fclose(disk);
	free(writeBuffer);
	free(buffer);
}




/******
	Initialize the Little Log File System
	
	Wipe the vdisk
	
	Initialize the Superblock with magic number 18
	
	Initializie the freeBlock vector as Block 1 (Blocks 0-9 unavailable)
	
	Start the root directory as the firt free block (10);
	*******/
void InitLLFS()
{
	FILE* disk = fopen(vdisk_path, "wb");
	char* init = calloc(BLOCK_SIZE * NUM_BLOCKS, 1);
	fwrite(init, BLOCK_SIZE * NUM_BLOCKS, 1, disk);
	free(init);
	fclose(disk);
	
	disk = fopen(vdisk_path, "rb+");
	
	if (disk == NULL)
	{
		printf("Disk not found!\n");
		return;
	}
	
	//Initialize the superblock
	char* buffer;
	buffer = (char*) malloc(BLOCK_SIZE);
	int magic = 18;
	int blocks = NUM_BLOCKS;
	int inodes = NUM_INODES;
	
	memset(buffer, 0, BLOCK_SIZE);
	memcpy(buffer, &magic, sizeof(magic));
	memcpy(buffer + sizeof(int) * 1, &blocks, sizeof(int));
	memcpy(buffer + sizeof(int) * 2, &inodes, sizeof(int));
	writeBlock(disk, 0, buffer);
	
	//Initialize the freeblock vector
	char* buffer2;
	buffer2 = (char*) malloc(BLOCK_SIZE);
	
	int unavailable1 = 0;
	int unavailable2 = 63;
	int available = 255;
	
	memcpy(buffer2, &unavailable1, 1);
	memcpy(buffer2 + 1, &unavailable2, 1);
	
	for (int i = 0; i < 510; i++)
		memcpy(buffer2 + 2 + i, &available, 1);
	
	writeBlock(disk, FREE_BLOCK, buffer2);
	
	fclose(disk);
	free(buffer2);
	free(buffer);
	//end superblock and freeBlock vector
	
	//start root directory
	disk = fopen(vdisk_path, "rb+");
	
	struct inode root_inode;
	root_inode.type = DIRECTORY;
	root_inode.num = 1;
	root_inode.blocks[0] = firstFreeBlock();
	
	
	//initailize to 0's
	for (int i = 1; i < 20; i++)
		root_inode.blocks[i] = 0;
	
	curDirectoryBlock = root_inode.blocks[0];
	root_inode.fileSize = 0;
	
	
	
	char* buffer3;
	buffer3 = (char*) malloc(BLOCK_SIZE);
	memset(buffer3, 0, BLOCK_SIZE);
	memcpy(buffer3, &root_inode.fileSize, 4);
	memcpy(buffer3 + 4, &root_inode.type, 4);
	memcpy(buffer3 + 8, &root_inode.blocks, 20);
	
	writeBlock(disk, 2, buffer3);
	
	fclose(disk);
	free(buffer3);
	//end root directory
	
	inode++;
}







