#include <string.h>
#include "stdio.h"

void codecA_func(char *string) {
    for (size_t i = 0;i<strlen(string) ; i++) {
        if (string[i] >= 'a' && string[i] <= 'z') {
            string[i] = (char) (string[i] - 'z' + 'Z');
        } else if (string[i] >= 'A' && string[i] <= 'Z') {
            string[i] = (char) (string[i] - 'Z' + 'z');
        }
    }
    printf("%s\n", string);
}


void codecA_decode(char *str) {
    codecA_func(str);
}