#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>

#include <sandwich/crypto.h>

int main (int argc, char** argv)
{
	if (argc !=6)
	{
		printf ("Usage:\n %s keys/private.pem keys/ca_cert.pem keys/shop_cert.pem keys/global_pkey.pem keys/global_pubkey.pem\n", argv[0]);
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
	int res = verify_certificate (argv[3], argv[2]);
	if (res > 0)
		printf ("Certificate '%s' is signed by CA with certificate '%s'!\n", argv[3], argv[2]);


	uint8_t k_tag[16]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xee };
	uint8_t *crypted = malloc (RSA_size(global_public));
	res = RSA_public_encrypt (16, k_tag, (unsigned char*) crypted, global_public, RSA_PKCS1_PADDING);
	if (res < 0)
		fprintf (stderr, "Something went wrong while ciphering\n");
	printf ("Encrypted key has length %d\n", RSA_size(global_public));

	unsigned int digestlen;
	unsigned char *digest = digest_message (crypted, &digestlen);
	uint8_t *signature = malloc (RSA_size(shop_private));
	res = RSA_sign (NID_sha1, digest, digestlen,  (unsigned char *) signature, &siglen , shop_private);
	if (res <= 0)
	{
		fprintf (stderr, "Something went wrong while signing\n");
		long err = ERR_peek_last_error();	
		const char* errbuf = ERR_reason_error_string(err);
		fprintf (stderr, "Error: %s\n", errbuf);
	}

	RSA* our_pubkey = load_key_from_file ("/home/moe/code/sandbox/libfreefare/keys/myPubKey.pem", CRYPTO_PUBLIC);
	res = RSA_verify (NID_sha1, (unsigned char*) digest, digestlen, (unsigned char *) signature, siglen, our_pubkey);
	if (res <= 0)
		fprintf (stderr, "Something went wrong while signing, can't verify the thing with our pubkey\n");
	free(signature);
	free (digest);
	RSA_free(shop_private);
	X509_free(ca_cert);
	X509_free(shop_cert);


	return EXIT_SUCCESS;
}
