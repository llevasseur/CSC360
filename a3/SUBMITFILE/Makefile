CC := gcc
CFLAGS := -Wall -Werror

TESTS := test1 test2

all: $(TESTS)

test%: test%.c ../disk/disk.c ../disk/disk.h ../io/File.c ../io/File.h $(CC) $(CFLAGS) -I.. -o $@ ../disk/disk.c ../io/File.c -lncurses

