#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



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

int generate_random_key (uint8_t* buf, int len)
{
	return !RAND_pseudo_bytes ((unsigned char* )buf, len);
}


keyvault_t* create_keyvault_new (uint8_t *k_m_1, uint8_t *k_w_1, uint8_t *k_m_2, uint8_t *k)
{
	keyvault_t *ks = (keyvault_t *) malloc (sizeof(keyvault_t));
	memset ((void*) ks, 0, sizeof(keyvault_t));
	memcpy (ks->k_m_1, k_m_1, 16 * sizeof (uint8_t));
	memcpy (ks->k_w_1, k_w_1, 16 * sizeof (uint8_t));
	memcpy (ks->k_m_2, k_m_2, 16 * sizeof (uint8_t));
	memcpy (ks->k, k, 16 * sizeof (uint8_t));
	return ks;
}

void destroy_keyvault (keyvault_t* kv)
{
	free (kv);
}

int write_keyvault_to_file (const char* filename, keyvault_t* kv)
{
	/* FIXME: Encrypt this first */
	int fd = creat (filename, S_IRUSR | S_IWUSR);
	if (fd < 0)
		fprintf (stderr, "Could not open file %s\n", filename);
	else
	{
		write (fd, &kv->k_m_1, 16);
		write (fd, &kv->k_w_1, 16);
		write (fd, &kv->k_m_2, 16);
		write (fd, &kv->k, 16);
		close (fd);
	}
	printf ("Writing keyvault to file %s\n", filename);
	return EXIT_SUCCESS;
}

int init_keyvault_from_file (const char* filename, keyvault_t* kv)
{
	/* FIXME: Decrypt this first */
	int fd = open (filename, O_RDONLY);
	if (fd < 0)
		fprintf (stderr, "Could not open file %s\n", filename);
	else
	{
		if (read (fd, &kv->k_m_1, 16) != 16)
			fprintf (stderr, "Read error while reading keyvault\n");
		if (read (fd, &kv->k_w_1, 16) != 16)
			fprintf (stderr, "Read error while reading keyvault\n");
		if (read (fd, &kv->k_m_2, 16) != 16)
			fprintf (stderr, "Read error while reading keyvault\n");
		if (read (fd, &kv->k, 16) != 16)
			fprintf (stderr, "Read error while reading keyvault\n");
		close (fd);
	}
	printf ("Wrote keyvault to file %s\n", filename);
	return EXIT_SUCCESS;
}


