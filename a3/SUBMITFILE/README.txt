Leevon Levasseur V00868326
University of Victoria, CSC 360 A1
April 06, 2020

My LLFS can:

	- when InitLLF() is called
	
		- makes a fresh vdisk
		- set up the superblock with magic number 18, number of blocks 4096, number of inodes 128
		- Initialize the freeblock vector
		- Sets root directory to first free block
		- Gives root directory an inode (1)
	
	- Now the user can make a DIRECTORY file or a FLAT file

	- To make a DIRECTORY file, call makeDir(*name*)
		- This will make a directory inside the current directory
		- Takes the first free block from the freeBlockVector and makes it unavailable
		- Takes the next inode (no deleting function so inodes just increment)
		- stores inode information in the first free 32 bytes of the metadata blocks (2 -9)
		- references the parent directory as it's first entry (inode and name: "..")
	
	- To make a FLAT file, call makeFlat(*name*)
		- This will make a regular file inside the current directory
		- Takes the first free block from the freeBlockVector and makes it unavailable
		- Takes the next inode and stores information in the first free 32 bytes of the metadata blocks (2 -9)
	
	- To go into a directory or access a flat file call openFile(*name*)
		- the file must exist in the current directory
		- if the current type of the file we are in is a Flat (not currently in a directory) then return with an error message
		- otherwise, update global information, letting the filesytem know we are in the specific file
	
	- To write to a flat file, call writeToFile(*text*)
		- if the current type of the file we are in is a Directory (not a writable file) then return with an error message
		- otherwise, if the text + the previous cursor is less than a blocksize, write to the file's block


My LLFS cannot:
	
	- Delete a file
	- use a inode bitmap
	- write to multiple blocks



My LLFS bugs:
	
	-