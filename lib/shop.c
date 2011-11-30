#include <string.h>
#include <err.h>

#include <sandwich/shop.h>

void update_counter (MifareTag tag, const keyvault_t *kv, uint32_t value)
{
	int res;
	char output[32];
	memset (&output[0], 0, 32 * sizeof (char));
	sprintf (output, "%04x", value);

	MifareDESFireAID aid = mifare_desfire_aid_new (0x2);
	res = mifare_desfire_select_application(tag, aid);
	if (res < 0)
		errx (EXIT_FAILURE, "Application selection failed");
	free (aid);

	MifareDESFireKey key;
	if (get_keytype_3DES (kv, CRYPTO_KEY_K))
		key = mifare_desfire_3des_key_new_with_version ((unsigned char *) kv->k);
	else
		key = mifare_desfire_des_key_new_with_version ((unsigned char *) kv->k);
	mifare_desfire_key_free (key);

	res = mifare_desfire_authenticate (tag, 1, key);
	if (res < 0)
		freefare_perror (tag, "Authenticating to change counter");

	ssize_t written = mifare_desfire_write_data (tag, 0x01, 0x0, 0x20, output);
	if (written < 0)
		freefare_perror(tag, "Writing data to tag");
	else
		printf ("Wrote %ld bytes to counter ...\n", written);
}

uint32_t read_counter (MifareTag tag, const keyvault_t *kv)
{
	int res;
	char output[32];
	memset (&output[0], '\0', 32);

	MifareDESFireAID aid = mifare_desfire_aid_new (0x2);
	res = mifare_desfire_select_application(tag, aid);
	if (res < 0)
		errx (EXIT_FAILURE, "Application selection failed");
	free (aid);

	MifareDESFireKey key;
	if (get_keytype_3DES (kv, CRYPTO_KEY_K))
		key = mifare_desfire_3des_key_new_with_version ((unsigned char*) kv->k);
	else
		key = mifare_desfire_des_key_new_with_version ((unsigned char*) kv->k);

	res = mifare_desfire_authenticate (tag, 1, key);
	if (res < 0)
		freefare_perror (tag, "Authenticating to change counter");
	mifare_desfire_key_free (key);
	ssize_t read = mifare_desfire_read_data (tag, 0x1, 0x0, 0x20, output);
	if (read < 0)
		freefare_perror(tag, "Reading data from tag");
	else
		printf ("Read %ld bytes from counter ...\n", read);
	uint32_t ret;
	sscanf (output, "%x", &ret);
	return ret;
}

void write_log (MifareTag tag, const keyvault_t *kv, uint8_t *data, uint32_t count, RSA *shop_private)
{
	int res;
	(void) kv;
	(void) count;
	(void) tag;
	unsigned int digestlen = strlen ((char *) data);
	unsigned int siglen = RSA_size (shop_private);
	unsigned char *digest = digest_message (data, &digestlen);
	res = RSA_sign (NID_sha1, digest, digestlen,  (unsigned char *) data + 72, &siglen , shop_private);
	if (res <= 0)
		fprintf (stderr, "Something went wrong while signing\n");

}
