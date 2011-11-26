CC=gcc
LD=ld
CFLAGS=-Wall -std=c99 -Werror -O2
LDFLAGS=-L $(PWD) -lpthread -lsandwich
FREEFARECFLAGS=$(shell pkg-config --cflags libfreefare)
FREEFARELIBS=$(shell pkg-config --libs-only-l libfreefare)
OSSLLIBS=$(shell pkg-config --libs-only-l openssl)

default: initialise-card crypto-main


## Our main applications
initialise-card: initialise_card.o sandwich keyvaults
	$(CC) -o $@ $(FREEFARELIBS) $(LDFLAGS) $<

crypto-main: crypto_main.o sandwich
	$(CC) -o $@ $(OSSLLIBS) $(LDFLAGS) $<

crypto-main.o: crypto_main.c
	$(CC) -c -o $@ $(CFLAGS) $(FREEFARECFLAGS) $<

initialise_card.o: initialise_card.c
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
	-rm initialise-card
	-rm crypto-main
	-rm analyse-card
	-rm *.o
	-rm *.so

dist-clean:
	-rm -r keyvaults
