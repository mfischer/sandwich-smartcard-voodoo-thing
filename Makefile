CC=clang
LD=ld.gold
CFLAGS=-Wall -std=c99 -Werror -O2 -isystem include
LDFLAGS=-L lib -lpthread -lsandwich
FREEFARECFLAGS=$(shell pkg-config --cflags libfreefare)
FREEFARELIBS=$(shell pkg-config --libs-only-l libfreefare)
OSSLLIBS=$(shell pkg-config --libs-only-l openssl)

default: initialise-card crypto-main


## Our main applications
initialise-card: apps/initialise_card.o sandwich keyvaults
	$(CC) -o apps/$@ $(FREEFARELIBS) $(LDFLAGS) $<

initialise_card.o: apps/initialise_card.c
	$(CC) -c -o apps/$@ $(CFLAGS) $(FREEFARECFLAGS) $<

crypto-main: apps/crypto_main.o sandwich
	$(CC) -o apps/$@ $(OSSLLIBS) $(LDFLAGS) $<

crypto-main.o: crypto_main.c
	$(CC) -c -o apps/$@ $(CFLAGS) $(FREEFARECFLAGS) $<

## Our own library
setup: lib/setup.c include/sandwich/setup.h
	$(CC) -fPIC -c -o lib/$@.o $(CFLAGS) $<

log: lib/log.c include/sandwich/log.h
	$(CC) -fPIC -c -o lib/$@.o $(CFLAGS) $<

crypto: lib/crypto.c include/sandwich/crypto.h
	$(CC) -fPIC -c -o lib/$@.o $(CFLAGS) $<

sandwich: crypto log setup
	$(LD) -shared -o lib/libsandwich.so -lc lib/*.o $(OSSLLIBS) $(FREEFARELIBS)

keyvaults:
	-mkdir keyvaults

install:
	cp -r include $(INSTALL_PREFIX)
	cp lib/libsandwich.so $(INSTALL_PREFIX)/lib
	cp apps/initialise-card $(INSTALL_PREFIX)/bin

.PHONY: clean
clean:
	-rm apps/initialise-card
	-rm apps/crypto-main
	-rm apps/*.o
	-rm lib/*.o
	-rm lib/*.so

.PHONY: dist-clean
dist-clean: clean
	-rm -r keyvaults
