#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

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

int main (int argc, char** argv)
{
	if (argc !=6)
	{
		printf ("Usage:\n %s keyfile.pem\n", argv[0]);
		return EXIT_FAILURE;
	}
	RSA* shop_private = NULL;
	RSA* global_private = NULL;
	RSA* global_public = NULL;
	X509* ca_cert = NULL;
	X509* shop_cert = NULL;

	printf ("Private keyfile : %s\n", argv[1]);
	printf ("Ca Cert : %s\n", argv[2]);
	printf ("Shop Cert : %s\n", argv[3]);

	shop_private = load_key_from_file (argv[1], 1);
	ca_cert = load_cert_from_file (argv[2]);
	shop_cert = load_cert_from_file (argv[3]);

	global_private = load_key_from_file (argv[4], 1);
	global_public = load_key_from_file (argv[5], 0);

	/*RSA_print_fp(stdout, shop_private, 0);*/

	int info = 0;
	char *input  = "1122334455667788aabbccddeeff\0";
	char *output = malloc (RSA_size(global_public));
	info = RSA_public_encrypt (strlen(input), (unsigned char*) input, (unsigned char*) output, global_public, RSA_PKCS1_PADDING);
	if (info < 0)
		fprintf (stderr, "Something went wrong\n");
	else
		printf ("Encrypted length %d\n", info);

	char *final_output = malloc (sizeof(char) * info);
	info = RSA_private_decrypt (128, (unsigned char*) output, (unsigned char*) final_output, global_private, RSA_PKCS1_PADDING);
	if (info < 0)
		fprintf (stderr, "Something went wrong while decrypting\n");
	else
		printf ("Recovered plaintext %s\n", final_output);

	unsigned int siglen;
	info = RSA_sign (NID_sha1, (unsigned char*) input, strlen (input),  (unsigned char*) output, &siglen , shop_private);
	if (info < 0)
		fprintf (stderr, "Something went wrong when signing\n");
	else
		printf ("Signature %s (Len = %u)\n", output, siglen);

	info = RSA_verify (NID_sha1, (unsigned char*) input, strlen (input),  (unsigned char*) output, siglen , shop_private);
	if (info != 1)
		fprintf (stderr, "Something went wrong when verifying\n");
	else
		printf ("Signature ok!\n");


	free (final_output);
	free (output);
	RSA_free(shop_private);
	X509_free(ca_cert);
	X509_free(shop_cert);
	return EXIT_SUCCESS;
}
