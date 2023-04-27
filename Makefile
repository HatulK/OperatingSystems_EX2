CC = gcc
CFLAGS = -Wall -Wextra
LDFlags = -ldl
RPATH=-Wl,-rpath=./

all: cmp copy codecAlib.so codecBlib.so encode decode stshell

cmp: cmp.c
	$(CC) cmp.c -o cmp
copy: copy.c
	$(CC) copy.c -o copy

codecAlib.so: codecA.c
	$(CC) -c -fPIC $< $(CFLAGS) -o codecA.o
	$(CC) -shared -o $@ codecA.o

codecBlib.so: codecB.c
	$(CC) -c -fPIC $< $(CFLAGS) -o codecB.o
	$(CC) -shared -o $@ codecB.o

encode: encode.c
	$(CC) $< $(CFLAGS) -o $@ $(LDFLAGS) $(RPATH)

decode: decode.c
	$(CC) $< $(CFLAGS) -o $@ $(LDFLAGS) $(RPATH)

stshell: stshell.c
	$(CC) $< $(CFLAGS) -o $@ $(LDFLAGS)

clean:
	rm -f *.o cmp copy codecAlib.so codecBlib.so codecA.o codecB.o encode decode stshell