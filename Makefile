CC=clang
LD=ld.gold
CFLAGS=-Wall -std=gnu99 -Werror -O2 -isystem include -g
SC=swig
PYTHON_VERSION=2.7
LDFLAGS=-L lib -lpthread -lsandwich
FREEFARECFLAGS=$(shell pkg-config --cflags libfreefare)
FREEFARELIBS=$(shell pkg-config --libs-only-l libfreefare)
OSSLLIBS=$(shell pkg-config --libs-only-l openssl)
SWIGCFLAGS=$(shell python2-config --includes)

ifdef DESTDIR
	INSTALL_PREFIX=${DESTDIR}
else
	INSTALL_PREFIX=/usr/local
endif

default: initialise-card analyse-card crypto-main log_test buy swig-shop

## Our main applications
initialise-card: apps/initialise_card.o sandwich keyvaults
	$(CC) -o apps/$@ $(FREEFARELIBS) $(OSSLLIBS) $(LDFLAGS) $<

initialise_card.o: apps/initialise_card.c
	$(CC) -c -o apps/$@ $(CFLAGS) $(FREEFARECFLAGS) $<

crypto-main: apps/crypto_main.o sandwich
	$(CC) -o apps/$@ $(OSSLLIBS) $(LDFLAGS) $<

analyse-card: apps/analyse_card.o sandwich keyvaults
	$(CC) -o apps/$@ $(FREEFARELIBS) $(OSSLLIBS) $(LDFLAGS) $<

crypto-main.o: apps/crypto_main.c
	$(CC) -c -o apps/$@ $(CFLAGS) $(FREEFARECFLAGS) $<

analyse_card.o: apps/analyse_card.c
	$(CC) -c -o apps/$@ $(CFLAGS) $(FREEFARECFLAGS) $<

log_test: apps/log_test.c sandwich
	$(CC) -o apps/$@ $(CFLAGS) $(OSSLLIBS) $< $(LDFLAGS)

buy: apps/buy.c sandwich
	$(CC) -o apps/$@ $(FREEFARELIBS) $(CFLAGS) $(OSSLLIBS) $(LDFLAGS) $<

## Our own library
setup: lib/setup.c include/sandwich/setup.h
	$(CC) -fPIC -c -o lib/$@.o $(CFLAGS) $<

log: lib/log.c include/sandwich/log.h
	$(CC) -fPIC -c -o lib/$@.o $(CFLAGS) $<

crypto: lib/crypto.c include/sandwich/crypto.h
	$(CC) -fPIC -c -o lib/$@.o $(CFLAGS) $<

shop: lib/shop.c include/sandwich/shop.h
	$(CC) -fPIC -c -o lib/$@.o $(CFLAGS) $<

sandwich: crypto log setup shop
	$(LD) -shared -o lib/libsandwich.so -lc lib/*.o $(OSSLLIBS) $(FREEFARELIBS)


## The swig stuff
swig-shop: swig/swig_shop.i swig/swig_shop.c
	$(SC) -python -outdir swig/sandwich $<
	$(CC) -fPIC -c -o swig/swig_shop_wrap.o swig/swig_shop_wrap.c $(SWIGCFLAGS)
	$(CC) -fPIC -c -o swig/swig_shop.o swig/swig_shop.c $(SWIGCFLAGS) $(CFLAGS)
	$(LD) -shared -o swig/sandwich/_swig_shop.so -lc swig/*.o $(OSSLLIBS) $(FREEFARELIBS) $(LDFLAGS)


keyvaults:
	-mkdir keyvaults

install: default
	install -d include/sandwich $(INSTALL_PREFIX)/include/sandwich
	install include/sandwich/*h $(INSTALL_PREFIX)/include/sandwich
	install -d lib $(INSTALL_PREFIX)/lib
	install lib/libsandwich.so $(INSTALL_PREFIX)/lib
	install -d swig/sandwich $(INSTALL_PREFIX)/lib/python${PYTHON_VERSION}/site-packages
	install swig/sandwich/__init__.py $(INSTALL_PREFIX)/lib/python${PYTHON_VERSION}/site-packages/sandwich
	install swig/sandwich/Log.py $(INSTALL_PREFIX)/lib/python${PYTHON_VERSION}/site-packages/sandwich
	install swig/sandwich/_swig_shop.so $(INSTALL_PREFIX)/lib/python${PYTHON_VERSION}/site-packages/sandwich
	test -d ${INSTALL_PREFIX}/bin || mkdir ${INSTALL_PREFIX}/bin
	install apps/initialise-card $(INSTALL_PREFIX)/bin/initialise-card
	install apps/analyse-card $(INSTALL_PREFIX)/bin/analyse-card
	install apps/buy $(INSTALL_PREFIX)/bin/buy

.PHONY: clean
clean: clean-swig
	-rm apps/initialise-card
	-rm apps/crypto-main
	-rm apps/analyse-card
	-rm apps/*.o
	-rm lib/*.o
	-rm lib/*.so

.PHONY: clean-swig
clean-swig:
	-rm swig/swig_shop_wrap.c
	-rm swig/sandwich/swig_shop.py
	-rm swig/swig_shop_wrap.o

.PHONY: dist-clean
dist-clean: clean
	-rm -r keyvaults
