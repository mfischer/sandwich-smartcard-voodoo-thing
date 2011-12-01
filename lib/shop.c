#include <string.h>
#include <err.h>

#include <sandwich/log.h>
#include <sandwich/shop.h>

void update_counter (MifareTag tag, const keyvault_t *kv, uint32_t value)
{
	int res;
	char output[32];
	memset (&output[0], 0, 32 * sizeof (char));
	sprintf (output, "%04u", value);

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
	sscanf (output, "%u", &ret);
	return ret;
}

void write_log (MifareTag tag, const keyvault_t *kv, char *shop_name, uint32_t count, RSA *shop_private)
{
	char data[200];
	int res;
	generate_log (shop_name, count, data, shop_private);

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

	ssize_t written = mifare_desfire_write_data (tag, 0x02, 0xc8 * ((count-1) % 10), 0xc8, data);
	if (written < 0)
		freefare_perror(tag, "Writing data to tag");
	else
		printf ("Wrote %ld bytes to log ...\n", written);
}

void read_log_entry (MifareTag tag, const keyvault_t *kv, size_t number, char *logentry)
{
	int res;
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
		freefare_perror (tag, "Authenticating to read logentries");
	mifare_desfire_key_free (key);
	ssize_t read = mifare_desfire_read_data (tag, 0x2, 0xc8 * number, 0xc8, logentry);
	if (read < 0)
		freefare_perror(tag, "Reading data from tag");
}
