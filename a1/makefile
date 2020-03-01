.phoney all:
all: SEEsh doubletroublelist

SEEsh: SEEsh.c
	gcc -std=c11 -Wall -Werror SEEsh.c -o SEEsh

doubletroublelist: doubletroublelist.c
	gcc -std=c11 -Wall -Werror -pedantic-errors doubletroublelist.c -o dList
