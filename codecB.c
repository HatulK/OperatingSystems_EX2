#include <stdio.h>
#include <string.h>

void codecB_func(char *str) {
    for (size_t i = 0; i<strlen(str); i++) {
        unsigned char c = str[i];
        c += 3;
        str[i] = (char) c;
    }
    printf("%s\n", str);
}

void codecB_decode(char *str) {
    for (size_t i = 0; i<strlen(str); i++) {
        unsigned char c = str[i];
        c -= 3;
        str[i] = (char) c;
    }
    printf("%s\n", str);
}