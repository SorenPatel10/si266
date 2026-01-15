#include <stdio.h>
#include <string.h>

struct Box {
    char buffer[8];         
    int secret_code;     
};

int main(){
    struct Box b;
    b.secret_code = 0;
    printf("%X\n", b.secret_code);
    strcpy(b.buffer,"12345678ABCD");
    printf("%X\n", b.secret_code);
}

/*
b.buffer is 8 bytes and we are writing 13 bytes into it (12-char string plus null byte).
Numbers 1-8 get copied into the buffer, but it gets overflowed, and ABCD goes into b.secret_code.
This gets placed with corresponding bytes 41 42 43 44 into b.secret_code,
which is displayed backwards as a hex number 0x44434241 due to little endianness.
*/