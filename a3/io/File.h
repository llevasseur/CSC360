#ifndef FILE_H
#define FILE_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>

void makeDir(char* name);

void makeFlat(char* name);

void openFile(char* name);

void closeFile();

void writeToFile(char* text);

void InitLLFS();

#endif