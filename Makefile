CC=clang
CFLAGS=-Wall -g -std=c99
FREEFARECFLAGS=$(shell pkg-config --cflags libfreefare)
FREEFARELIBS=$(shell pkg-config --libs libfreefare)
OPENSSLLIBS=$(shell pkg-config --libs openssl)

default: main log.o crypto

main: main.c log.o
	$(CC) -o main $(CFLAGS) $(FREEFARECFLAGS) $(FREEFARELIBS) $^

log.o: log.c
	$(CC) -c -o $@ $(CFLAGS) $(LIBS) $<

crypto: crypto.c
	$(CC) -o $@ $(CFLAGS) $(LIBS) $(OPENSSLLIBS) $<

.PHONY: clean

clean:
	-rm main
	-rm crypto
	-rm *.o

#-rm *.so
#-rm *.py
#-rm *.pyc

#swig: freefare.i
#swig -python $<
#$(CC) -fPIC -c freefare_wrap.c $(LIBS) -I /usr/include/python2.7
#ld -shared freefare_wrap.o -o _freefare.so $(LIBS)
