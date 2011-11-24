#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

#include "crypto.h"

int verify_certificates (X509 *cert, X509 *ca)
{
	X509_STORE *store = X509_STORE_new();
	X509_STORE_CTX *verify_ctx = X509_STORE_CTX_new();

	STACK_OF(X509) *st = sk_X509_new_null();
	sk_X509_push (st,ca);
	sk_X509_push (st,cert);

	X509_STORE_CTX_init(verify_ctx, store, cert ,st);
	int ret = X509_verify_cert(verify_ctx);
	sk_X509_free (st);
	return ret;
}

RSA* load_key_from_file (const char *filename, int private)
{
	FILE* keyfile;
	RSA* ret = NULL;
	
	printf ("Loading file %s ...\n", filename);
	if ( !(keyfile = fopen (filename, "r")) )
		fprintf (stderr, "Something went wrong when opening file %s\n", filename);
	if (private)
		ret = PEM_read_RSAPrivateKey (keyfile, NULL, NULL, NULL);
	else
	{
		ret = PEM_read_RSA_PUBKEY (keyfile, NULL, NULL, NULL);
	}
	if (!ret)
		fprintf (stderr, "Something went wrong when reading the key from %s\n", filename);
	fclose (keyfile);
	return ret;
}

X509* load_cert_from_file (const char *filename)
{
	FILE* certfile;
	X509* ret;
	printf ("Loading file %s ...\n", filename);
	if ( !(certfile = fopen (filename, "r")) )
		fprintf (stderr, "Something went wrong when opening file %s\n", filename);
	ret = PEM_read_X509 (certfile, NULL, NULL, NULL);
	fclose (certfile);
	return ret;
}
