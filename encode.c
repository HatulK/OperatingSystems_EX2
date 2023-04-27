//
// Created by vboxuser on 4/25/23.
//
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

typedef void (*func)(const char *);


void *getEncoder(const char *codec) {
    char name[512];
    sprintf(name, "%slib.so", codec);
    void *handler = dlopen(name, RTLD_LAZY);
    if (!handler) {
        fprintf(stderr, "Error while loading library: %s_lib\n", dlerror());
        exit(1);
    }
    return handler;
}

void exeEncode(const char *name, const char *message, void *handler) {
    char function[512];
    sprintf(function, "%s_func", name);
    func encoder = dlsym(handler, function);
    if (!encoder) {
        fprintf(stderr, "Error while loading function: %s\n", dlerror());
        dlclose(handler);
        exit(2);
    }
    encoder(message);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Please make sure to follow the format as shown below:");
        fprintf(stderr, "%s <codec> <string to en/decode>\n", argv[0]);
        return -1;
    }
    char *codec = argv[1];
    char *string = argv[2];
    void *handler = getEncoder(codec);
    exeEncode(codec, string, handler);
    dlclose(handler);
    return 0;
}