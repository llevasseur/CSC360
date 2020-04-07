CC := gcc
CFLAGS := -g -Wall -Wno-deprecated-declarations -Werror

TESTS := test01

all: $(TESTS)


test%: test%.c ../File.c ../File.h
	$(CC) $(CFLAGS) -I.. -o $@ $< ../File.c
