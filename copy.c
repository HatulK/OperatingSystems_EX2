#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


int fileCopier(char *name1, char *name2, int v, int f) {
    int exist = access(name2,F_OK);
    if(!exist){
        if(!f){
            if(v) printf("File is already exist and f flag is off");
            exit(1);
        }
    }
    char buffer[1024];
    int numofbyets;
    FILE *file1 = fopen(name1,"rb");
    FILE *file2 = fopen(name2,"wb");
    if(file1==NULL||file2==NULL){
        if(v) printf("Error has happened openning files.\n");
        return 1;
    }
    while((numofbyets= fread(buffer,1,sizeof(buffer),file1))>0){
        fwrite(buffer,1,numofbyets,file2);
    }
    fclose(file2);
    fclose(file1);
    if(v) printf("File has been copied\n");
    return 0;
}

void sendError() {
    printf("An error has happened while trying to run the program.\n");
    printf("Please make sure to use the following format:\n");
    printf("./copy <file_name1> <file_name2> -f -v \n");
    printf("-v flag: Verbose output. -f flag: overwrite file.\n");
    printf("Using flags is optional.\n");
    exit(1);
}

void checkInput(int argc, char *const *argv, int *v, int *f) {
    int result1, result2;
    *v = *f =result2=result1=-1;
    if(argc<3||argc>5) sendError();
    if(argc==4){
        result1= strcmp(argv[3],"-f");
        result2= strcmp(argv[3],"-v");
        if(!result1) *f=1;
        else if(!result2) *v=1;
        else sendError();
    }
    if(argc==3) *v=*f=0;
    if(argc==5){
        result1= strcmp(argv[3],"-v");
        result2= strcmp(argv[4],"-f");
        if(!result1&&!result2){
            *v=*f=1;
        } else{
            result1= strcmp(argv[3],"-f");
            result2= strcmp(argv[4],"-v");
            if(!result2&&!result1){
                *v=*f=1;
            }else sendError();
        }
    }
}

int main(int argc, char *argv[]) {
    int v, f;
    checkInput(argc, argv, &v, &f);
    char *name1 = argv[1];
    char *name2 = argv[2];
    return fileCopier(name1, name2, v, f);
}