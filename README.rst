===========================
 Sandwich FidelityCard App
===========================
This is a sample application for a sandwich fidelity card built with libfreefare / libnfc
using Mifare DESFire cards.

Dependencies:
~~~~~~~~~~~~~~

In order to build and use this software, you'll need at least:

	* openssl library including development headers
	* libnfc 1.5.1
	* libfreefare 0.3.2
	* pkg-config files should be in place for the above named libs
	* swig (tested with 2.0.4) (to generate the python bindings)
	* gcc (tested with 4.6.2) or llvm/clang
	* pyGtk (for the graphical interface)
	* python2 (tested with python 2.7)
	* make (tested with GNU Make 3.82)
	* pcsclite pcscd etc ... if your reader is not directly supported by libnfc

Building
~~~~~~~~~
After obtaining the code from tarball::

  user$: cd sandwich
  user$: make
  user$: sudo make install

Extracting the public keys
~~~~~~~~~~~~~~~~~~~~~~~~~~~
In order to extract the public keys the tool 'extract-public-keys' in the subfolder 'tools' can be used. To invoke it ::

  user$: ./tools/extract-public-keys directory-with-certs ca-file output-directory

Each of the certificates will be verified using the given CA certificate. After invoking the command the extracted public keys in PEM format can  be found in 'output-directory/certs'.

Running
~~~~~~~~
Please note, you have to obtain the keys / certificates prior to running the application.
To run the applications without installing them you can do the following after building::

  user$: export LD_LIBRARY_PATH=$PWD/lib

However this assumes that you're in the directory where you extracted the sources to.

Initialising / Erasing a card 
------------------------------
To initialise a card type::

  user$: ./apps/initialise-card [global_public.pem] [global_private.pem] [shop_public.pem] [shop_private.pem]

Where global_public.pem denotes the file containing the public key known to all shops, global_private.pem the private key known to all shops,
shop_public.pem and shop_private.pem denote the keys known only to the issuing shop.

Analysing a card
-----------------
To analyse a card's contents, type::

	user$: ./apps/analyse-card keys/global_private.pem keys

Where global_private.pem denotes the file containing the private key known to all shops, and keys the directory containing the public keys
extracted from the shops before in PEM format.

Buying a sandwich
------------------
To increase the counter / buy a sandwich, type::

	user$: ./apps/buy keys/global_private.pem keys/shop_private.pem

Where global_private.pem denotes the file containing the private key known to all shops, and shop_private.pem the key known to the shop where
the sandwich is about to be bought.

Graphical version
-----------------
We also designed a more user-friendly version of the software. (It requires Python 2.7 and gtk3 bindings for Python.)

To use the graphical version::

	user$: cd swig
	user$: python nice_gui.py

