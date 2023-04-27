//
// Created by vboxuser on 4/25/23.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int fileComperator(char *name1, char *name2, int v, int i) {
    int char1, char2;
    FILE *file1, *file2;
    file1 = fopen(name1, "rb");
    file2 = fopen(name2, "rb");
    if (file1 == NULL || file2 == NULL) {
        if (v) printf("Failed\n");
        return 1;
    }
    char1 = fgetc(file1);
    char2 = fgetc(file2);
    if (i) {
        char2 = tolower(char2);
        char1 = tolower(char1);
    }
    while (char1 != EOF || char2 != EOF) { //not sure if || or &&
        if (i) {
            if (char1 >= 'A' &&char1 <= 'Z') {
                char1 = (char1 - 'Z' + 'z');
            }
            if (char2 >= 'A' &&char2 <= 'Z') {
                char2 =  (char2 - 'Z' + 'z');
            }
        }
        if (char1 != char2) {
            if (v) printf("Distinct\n");
            fclose(file2);
            fclose(file1);
            return 1;
        }
        char1 = fgetc(file1);
        char2 = fgetc(file2);
    }
    fclose(file2);
    fclose(file1);
    if (v) {
        printf("Equal\n");
    }
    return 0;
}

void sendError() {
    printf("An error has happened while trying to run the program.\n");
    printf("Please make sure to use the following format:\n");
    printf("./cmp <filename1> <filename2> -i -v \n");
    printf("-v flag: Verbose output. -i flag: ignore cases.\n");
    printf("Using flags is optional.\n");
    exit(1);
}

void checkInput(int argc, char *const *argv, int *v, int *i) {
    int result, result2;
    if (argc < 3 || argc > 5) {
        sendError();
    }
    if (argc == 4) {
        result = strcmp("-v", argv[3]);
        result2= strcmp("-i", argv[3]);
        if (result == 0){
            (*v) = 1;
        }
        else if(result2==0){
            (*i)=1;
        }
        else {
            sendError();
        }
    }
    if (argc == 5) {
        result = strcmp("-v", argv[3]);
        result2 = strcmp("-i", argv[4]);
        if (result == 0 && result2 == 0) (*v) = (*i) = 1;
        else {
            result = strcmp("-i", argv[3]);
            result2 = strcmp("-v", argv[4]);
            if (result == 0 && result2 == 0) (*v) = (*i) = 1;
            else sendError();
        }
    }
}


int main(int argc, char *argv[]) {
    int v;
    int i;
    checkInput(argc, argv, &v, &i);
    char *name1 = argv[1];
    char *name2 = argv[2];
    int result = fileComperator(name1, name2, v, i);
    return result;
}


