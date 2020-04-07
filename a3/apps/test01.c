#include "../io/File.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>

int main()
{
	InitLLFS();
	/*printf("\tmaking directory \"Surface\"\n");
	makeDir("Surface");
	
	printf("\n\tmaking directory \"Deeper\" in \"Surface\"\n");
	makeDir("Deeper");
	
	printf("\nOpening \"Deeper\"\n\n");
	openFile("Deeper");
	
	//printf("current directory inode: %d\n", curDirectoryInode);
	
	printf("\n\tcreating flat file \"Rock\"\n\n");
	makeFlat("Rock");

	
	printf("\nOpening \"Rock\"\n\n");
	openFile("Rock");
	
	printf("\nWritting to \"Rock\" \"I am rock\"\n\n");
	writeToFile("I am a rock!");
	
	printf("\tClosing \"Rock\"\n");
	closeFile();
	
	printf("\nReturn to surface by opening \"..\"\n");
	openFile("..");

	
	printf("\nGoing to try to go higher by opening \"..\"\n");
	openFile("..");
	
	printf("\nGoing to stress test a FLAT file\n");
	makeFlat("Big Tree");
	
	printf("\nOpening \"Rock\"\n\n");
	openFile("Big Tree");
	
	printf("\nWritting to \"Big Tree\" \"Leaf\" recursively\n\n");
	
	//writeToFile("Leaf");
	for (int i = 0; i < 256; i++)
	{
		writeToFile("Leaf");
	}
	
	
	closeFile();
	
	printf("Finished test 1!\n");*/
	
	return 0;
}
