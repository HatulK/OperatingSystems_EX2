#include <stdio.h>

void Bencode(char *str) {
    for (int i = 0; str[i] != EOF; i++) {
        unsigned char c = str[i];
        c += 3;
        str[i] = (char) c;
    }
    printf("%s\n", str);
}

void Bdecode(char *str) {
    for (int i = 0; str[i] != EOF; i++) {
        unsigned char c = str[i];
        c -= 3;
        str[i] = (char) c;
    }
    printf("%s\n", str);
}