#include <string.h>

#include <sandwich/shop.h>

void update_counter (MifareTag tag, keyvault_t *kv, uint32_t value)
{
}

uint32_t read_counter (MifareTag tag, keyvault_t *kv)
{
	return 0;
}

void write_log (MifareTag tag, keyvault_t *kv, uint8_t *data, uint32_t count, RSA *shop_private)
{
	int res;
	unsigned int digestlen = strlen ((char *) data);
	unsigned int siglen = RSA_size (shop_private);
	unsigned char *digest = digest_message (data, &digestlen);
	res = RSA_sign (NID_sha1, digest, digestlen,  (unsigned char *) data + 72, &siglen , shop_private);
	if (res <= 0)
		fprintf (stderr, "Something went wrong while signing\n");

}
