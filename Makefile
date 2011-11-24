CC=gcc
LD=ld
CFLAGS=-Wall -std=c99 -Werror -O2
LDFLAGS=-L $(PWD) -lpthread -lsandwich
FREEFARECFLAGS=$(shell pkg-config --cflags libfreefare)
FREEFARELIBS=$(shell pkg-config --libs-only-l libfreefare)
OSSLLIBS=$(shell pkg-config --libs-only-l openssl)

default: main crypto-main


## Our main applications
main: main.o sandwich keyvaults
	$(CC) -o $@ $(FREEFARELIBS) $(LDFLAGS) $<

crypto-main: crypto_main.o sandwich
	$(CC) -o $@ $(OSSLLIBS) $(LDFLAGS) $<

crypto-main.o: crypto-main.c
	$(CC) -c -o $@ $(CFLAGS) $(FREEFARECFLAGS) $<

main.o: main.c
	$(CC) -c -o $@ $(CFLAGS) $(FREEFARECFLAGS) $<

## Our own library
setup.o: setup.c setup.h
	$(CC) -fPIC -c -o $@ $(CFLAGS) $<

log.o: log.c log.h
	$(CC) -fPIC -c -o $@ $(CFLAGS) $<

crypto.o: crypto.c crypto.h
	$(CC) -fPIC -c -o $@ $(CFLAGS) $<

sandwich: crypto.o log.o setup.o
	$(LD) -shared -o libsandwich.so -lc $^ $(OSSLLIBS) $(FREEFARELIBS)

keyvaults:
	-mkdir keyvaults

.PHONY: clean
clean:
	-rm main
	-rm crypto-main
	-rm *.o
	-rm *.so
	-rm -r keyvaults


#-rm *.so
#-rm *.py
#-rm *.pyc

#swig: freefare.i
#swig -python $<
#$(CC) -fPIC -c freefare_wrap.c $(LIBS) -I /usr/include/python2.7
#ld -shared freefare_wrap.o -o _freefare.so $(LIBS)
