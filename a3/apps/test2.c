#include "../io/File.h"
#include "../disk/disk.h"

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
	
	makeDir("Surface");
	
	makeDir("Deeper");

	openFile("Deeper");

	makeFlat("Rock");

	openFile("Rock");
	
	writeToFile("I am a rock!");
	
	writeToFile("And I roll!")
	
	closeFile();
	
	openFile("Rock");
	
	writeToFile("I'm losing control!");
	
	closeFile();
	
	openFile("..");
	
	printf("Im back to the surface, but what happens if I go higher?\n");

	openFile("..");
	
	printf("Finished test 1!\n");
	
	return 0;
}