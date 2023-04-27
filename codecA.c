#include "stdio.h"

void Aencode(char *string) {
    for (int i = 0; string[i] != EOF; i++) {
        if (string[i] >= 'a' && string[i] <= 'z') {
            string[i] = (char) (string[i] - 'z' + 'Z');
        } else if (string[i] >= 'A' && string[i] <= 'Z') {
            string[i] = (char) (string[i] - 'Z' + 'z');
        }
    }
    printf("%s\n", string);
}


void Adecode(char *str) {
    Aencode(str);
}