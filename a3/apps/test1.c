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

	makeDir("Hole");
	
	makeDir("Cave");
	
	openFile("Cave");
	
	makeDir("Cavern");
	
	MakeDir("Mine Shaft")
	
	OpenFile("Cavern");
	
	openFile("..");
	
	openFile("..");
	
	openFile("..");
	
	printf("I'm back to the surface!\n");
	
	printf("Finished test 1!\n");
	
	return 0;
}
