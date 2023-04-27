//
// Created by vboxuser on 4/25/23.
//
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

typedef char (*func)(const char *);


void *getDecoder(const char *codec) {
    char name[512];
    sprintf(name, "%slib.so", codec);
    void *handler = dlopen(name, RTLD_LAZY);
    if (!handler) {
        fprintf(stderr, "Error while loading library: %s\n", dlerror());
        exit(1);
    }
    return handler;
}

void exeDecode(const char *name, const char *message, void *handler) {
    char function[512];
    sprintf(function, "%s_decode", name);
    func decoder = dlsym(handler, function);
    if (!decoder) {
        fprintf(stderr, "Error while loading function: %s\n", dlerror());
        dlclose(handler);
        exit(1);
    }
    decoder(message);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Please make sure to follow the format as shown below:");
        fprintf(stderr, "%s <codec> <string> to en/decode>\n", argv[0]);
        return -1;
    }
    char *codec = argv[1];
    char *string = argv[2];
    void *handler = getDecoder(codec);
    exeDecode(codec, string, handler);
    dlclose(handler);
    return 0;
}