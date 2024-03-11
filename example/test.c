#include <stdio.h>

int test(){
    int ret = 5;
    return 6;
}

int main(){
    // Comment
    int num = 9;
    char ch = 'u';
    printf("Hello World");
    printf("Format Int: %d", 0);
    printf("Format Int: %d, Str: %s", 1, "TEST");
    printf("Format Int: %d, Str: %s, Char: %c", 2, "Testing", 'c');
    return 0;
}