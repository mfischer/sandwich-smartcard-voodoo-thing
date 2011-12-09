#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include <sandwich/log.h>
#include <sandwich/crypto.h>

int main (int argc, char** argv)
{
	char log_entry[200];
	RSA *shop_private = load_key_from_file (argv[1], CRYPTO_PRIVATE);
	RSA *shop_public = load_key_from_file (argv[2], CRYPTO_PUBLIC);
	generate_log ("Group1", 1024, log_entry, shop_private);
	char shop_name[58];
	unsigned char signature[128];
	uint32_t count;
	struct tm tm;
	parse_logentry (log_entry, &tm, shop_name, &count, signature);
	int ok = 0;

	unsigned int digestlen = 72;
	unsigned char* digest = digest_message ((uint8_t*) log_entry, &digestlen);
	ok = RSA_verify (NID_sha1, digest, digestlen,  (unsigned char*) log_entry + 72, 128, shop_public);
	printf ("Shop Name: %s\nCounter: %u\nSignature ok?: %u\n", shop_name, count , ok);

	return EXIT_SUCCESS;
}
