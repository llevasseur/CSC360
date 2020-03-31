#include <stdio.h>
#include <stdlib.h>

void print_binary(unsigned char byte)
{
    int k;
    for (int i = 7; i >= 0; i--)
    {
        k = byte >> i;
        
        if (k & 1)
            printf("1");
        else
            printf("0");
    }
    printf("\n");
}

void bitwise_and(unsigned char byte1, unsigned char byte2)
{
    unsigned char byte3 = byte1 & byte2;
    printf("AND\n");
    print_binary(byte1);
    print_binary(byte2);
    printf("--------\n");
    print_binary(byte3);
}

void bitwise_or(unsigned char byte1, unsigned char byte2)
{
    unsigned char byte3 = byte1 | byte2;
    printf("OR\n");
    print_binary(byte1);
    print_binary(byte2);
    printf("--------\n");
    print_binary(byte3);
}

void bitwise_xor(unsigned char byte1, unsigned char byte2)
{
    unsigned char byte3 = byte1 ^ byte2;
    printf("XOR\n");
    print_binary(byte1);
    print_binary(byte2);
    printf("--------\n");
    print_binary(byte3);
}

void bitwise_left_shift(unsigned char byte1, unsigned char amount)
{
    printf("SHIFT LEFT\n");
    unsigned char byte2 = byte1 << amount;
    print_binary(byte1);
    printf("shifted left %d\n", amount);
    print_binary(byte2);
}

void bitwise_right_shift(unsigned char byte1, unsigned char amount)
{
    printf("SHIFT RIGHT\n");
    unsigned char byte2 = byte1 >> amount;
    print_binary(byte1);
    printf("shifted right %d\n", amount);
    print_binary(byte2);
}

void bitwise_ones_complement(unsigned char byte1)
{
    printf("ONES COMPLEMENT\n");
    unsigned char byte2 = ~byte1;
    print_binary(byte1);
    print_binary(byte2);
}

int main()
{
    unsigned char byte1 = 0b10101100;    
    unsigned char byte2 = 0b11110000; 
    
    bitwise_and(byte1, byte2);
    printf("\n");
    bitwise_or(byte1, byte2);
    printf("\n");
    bitwise_xor(byte1, byte2);
    printf("\n");
    bitwise_left_shift(byte2, 8);
    printf("\n");
    bitwise_right_shift(byte2, 3);
    printf("\n");
    bitwise_ones_complement(byte1);
    printf("\n");
}
