#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



#include <sandwich/crypto.h>

unsigned char* digest_message (uint8_t *message, unsigned int *len)
{
	EVP_MD_CTX mdctx;
	const EVP_MD *md = EVP_sha1();
	unsigned char *digest = (unsigned char* ) malloc (sizeof(unsigned char) * EVP_MAX_MD_SIZE);
	EVP_MD_CTX_init(&mdctx);
	EVP_DigestInit_ex(&mdctx, md, NULL);
	EVP_DigestUpdate(&mdctx, (char *) message, *len);
	EVP_DigestFinal_ex(&mdctx, digest, (unsigned int *) len);
	EVP_MD_CTX_cleanup(&mdctx);
	return digest;
}

int check (X509_STORE *ctx, const char *file)
{
	X509 *cert = NULL;
	int res = 0;
	X509_STORE_CTX *csc;
	cert = load_cert_from_file (file);

	if (!cert)
		return 0;

	csc = X509_STORE_CTX_new();
	if (!csc)
	{
		if (cert)
			X509_free(cert);
		return 0;
	}

	X509_STORE_set_flags(ctx, 0);
	if(!X509_STORE_CTX_init(csc, ctx, cert, 0))
	{
		if (cert)
			X509_free(cert);
		if (csc)
			X509_STORE_CTX_free(csc);
		return 0;
	}
	res = X509_verify_cert(csc);
	X509_STORE_CTX_free(csc);
	X509_free(cert);
	return (res > 0);
}

int verify_certificate (const char* certfile, const char* ca_cert)
{
	X509_STORE *cert_ctx=NULL;
	X509_LOOKUP *lookup=NULL;

	cert_ctx=X509_STORE_new();
	if (!cert_ctx)
		return 0;

	OpenSSL_add_all_algorithms();

	lookup=X509_STORE_add_lookup(cert_ctx,X509_LOOKUP_file());
	if (!lookup)
	{
		if (cert_ctx)
			X509_STORE_free(cert_ctx);
		return 0;
	}

	if(!X509_LOOKUP_load_file(lookup,ca_cert,X509_FILETYPE_PEM))
	{
		if (cert_ctx)
			X509_STORE_free(cert_ctx);
		return 0;
	}

	lookup=X509_STORE_add_lookup(cert_ctx,X509_LOOKUP_hash_dir());
	if (!lookup)
	{
		if (cert_ctx)
			X509_STORE_free(cert_ctx);
		return 0;
	}

	X509_LOOKUP_add_dir(lookup,NULL,X509_FILETYPE_DEFAULT);

	return check(cert_ctx, certfile);
}



RSA* load_key_from_file (const char *filename, int private)
{
	FILE* keyfile;
	RSA* ret = NULL;
	
	#ifdef DEBUG
	printf ("Loading file %s ...\n", filename);
	#endif
	if ( !(keyfile = fopen (filename, "r")) )
	{
		fprintf (stderr, "Something went wrong when opening file %s\n", filename);
		return NULL;
	}
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
	#ifdef DEBUG
	printf ("Loading file %s ...\n", filename);
	#endif
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
	ks->version = CRYPTO_CUR_KEYVAULT_VERSION;
	return ks;
}

keyvault_t* create_keyvault_new_empty ()
{
	keyvault_t *ks = (keyvault_t *) malloc (sizeof(keyvault_t));
	memset ((void*) ks, 0, sizeof(keyvault_t));
	ks->version = CRYPTO_CUR_KEYVAULT_VERSION;
	return ks;
}

void destroy_keyvault (keyvault_t* kv)
{
	free (kv);
}

void set_keytype_3DES (keyvault_t *kv, uint8_t keyno)
{
	kv->keytypes = (0x80 >> keyno) | kv->keytypes;
}

void set_keytype_DES (keyvault_t *kv, uint8_t keyno)
{
	kv->keytypes = ~(0x80 >> keyno) & kv->keytypes;
}


void set_keytype_crypted (keyvault_t *kv, uint8_t keyno)
{
	kv->keytypes = (0x08 >> keyno) | kv->keytypes;
}

void set_keytype_non_crypted (keyvault_t *kv, uint8_t keyno)
{
	kv->keytypes = ~(0x08 >> keyno) & kv->keytypes;
}

uint8_t get_keytype_3DES (const keyvault_t *kv, uint8_t keyno)
{
	return (0x80 >> keyno) & kv->keytypes;
}

int write_keyvault_to_file (const char* filename, const keyvault_t* kv)
{
	/* FIXME: Encrypt this first */
	int fd = creat (filename, S_IRUSR | S_IWUSR);
	if (fd < 0)
		fprintf (stderr, "Could not open file %s\n", filename);
	else
	{
		/* FIXME: handle errors here ... */
		ssize_t res;
		res = write (fd, (const void *) &(kv->version), sizeof(uint8_t));
		res = write (fd, kv->k_m_1, 16 * sizeof (uint8_t));
		res = write (fd, kv->k_w_1, 16 * sizeof (uint8_t));
		res = write (fd, kv->k_m_2, 16 * sizeof (uint8_t));
		res = write (fd, kv->k, 16 * sizeof (uint8_t));
		res = write (fd, (const void *) &(kv->keytypes), sizeof (uint8_t));
		(void) res;
		close (fd);
	}
	printf ("Writing keyvault to file %s, KV version %02x\n", filename, kv->version);
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
		if (read (fd, (void *) &kv->version, sizeof (uint8_t)) != sizeof (uint8_t))
			fprintf (stderr, "Read error while reading keyvault\n");
		if (read (fd, kv->k_m_1, 16 * sizeof (uint8_t)) != 16 * sizeof (uint8_t))
			fprintf (stderr, "Read error while reading keyvault\n");
		if (read (fd, kv->k_w_1, 16 * sizeof (uint8_t)) != 16 * sizeof (uint8_t))
			fprintf (stderr, "Read error while reading keyvault\n");
		if (read (fd, kv->k_m_2, 16 * sizeof (uint8_t)) != 16 * sizeof (uint8_t))
			fprintf (stderr, "Read error while reading keyvault\n");
		if (read (fd, kv->k, 16 * sizeof (uint8_t)) != 16 * sizeof (uint8_t))
			fprintf (stderr, "Read error while reading keyvault\n");
		if (read (fd, (void *) &kv->keytypes, sizeof (uint8_t)) != sizeof (uint8_t))
			fprintf (stderr, "Read error while reading keyvault\n");
		close (fd);
	}
	printf ("Wrote keyvault to file %s\n", filename);
	return EXIT_SUCCESS;
}
