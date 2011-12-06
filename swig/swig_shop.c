#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <time.h>
#include <sys/time.h>

#include <nfc/nfc.h>
#include <freefare.h>

#include <sandwich/crypto.h>
#include <sandwich/shop.h>
#include <sandwich/log.h>

char *read_log_python (char *global_priv_file,
									           char *keydir, int number,
														 int *out_sigok, int *out_count)
{
	uint8_t k_tag_crypted[128];

	nfc_device_t *device = NULL;
	int error = EXIT_SUCCESS;
	int res;
	size_t device_count;
	MifareTag *tags = NULL;
	nfc_device_desc_t devices;

	nfc_list_devices (&devices, 1, &device_count);
	if (!device_count)
		errx (EXIT_FAILURE, "No NFC device found.");

	device = nfc_connect (&devices);
	if (!device)
	{
		warnx ("nfc_connect() failed.");
		error = EXIT_FAILURE;
	}

	tags = freefare_get_tags (device);
	if (!tags) {
		nfc_disconnect (device);
		errx (EXIT_FAILURE, "Error listing tags.");
	}
	char *tag_uid = freefare_get_tag_uid (tags[0]);

	if (DESFIRE != freefare_get_tag_type (tags[0]))
	{
		printf ("Error: Tag is not a DESFire\n");
		nfc_disconnect (device);
		errx (EXIT_FAILURE, "Error listing tags.");
	}
	res = mifare_desfire_connect (tags[0]);

	MifareDESFireAID aid = mifare_desfire_aid_new (0x1);
	res = mifare_desfire_select_application(tags[0], aid);
	if (res < 0)
		errx (EXIT_FAILURE, "Application selection failed");
	free (aid);

	int read = 0;
	read = mifare_desfire_read_data (tags[0], 0x01, 0x00, 0x80, k_tag_crypted);
	if (read < 0)
		freefare_perror (tags[0], "Reading data");

	keyvault_t* kv = create_keyvault_new_empty ();
	int keylen = 0;
	RSA *private = load_key_from_file (global_priv_file, CRYPTO_PRIVATE);
	keylen = RSA_private_decrypt (read, (unsigned char *) k_tag_crypted, kv->k, private, RSA_PKCS1_PADDING);
	if (keylen == 16)
		set_keytype_3DES (kv, CRYPTO_KEY_K);

	aid = mifare_desfire_aid_new (0x2);
	res = mifare_desfire_select_application(tags[0], aid);
	if (res < 0)
		errx (EXIT_FAILURE, "Application selection failed");
	free (aid);


	char logentry[LOG_MAX_LEN + 2 + 6];
	uint32_t counter = read_counter (tags[0], kv);
	/*uint32_t _counter = (counter >= LOG_MAX_ENTRIES) ? LOG_MAX_ENTRIES : counter;*/
	struct tm tm;
	char time[128];
	char shop_name[LOG_MAX_SHOP_LEN];
	uint32_t count;
	unsigned char signature[128];
	char shop_keyfile[255];

	/* Entry zero */
	read_log_entry (tags[0], kv, number, logentry);
	parse_logentry (logentry, &tm, shop_name, &count, signature);
	sprintf(shop_keyfile, "%s/public_keys/%s.pem", keydir, shop_name);
	RSA *key = load_key_from_file (shop_keyfile, CRYPTO_PUBLIC);
	int sigok = 0;
	if (!key)
		fprintf(stderr, "Could not find / open file for %s (%s)\n", shop_name, shop_keyfile);
	else
	{
		unsigned int digestlen = LOG_PAYLOAD_LEN;
		unsigned char *digest = digest_message ((uint8_t*) logentry, &digestlen);
		sigok |= RSA_verify (NID_sha1, digest, digestlen, signature, RSA_size(key) , key);
		free (digest);
		free (key);
	}
	*out_sigok = sigok;
	*out_count = counter;

	char *out_logentry = (char *) malloc (LOG_PAYLOAD_LEN);
	memcpy (out_logentry, logentry, LOG_PAYLOAD_LEN);

	
	(void) error;
	(void) time;
	RSA_free (private);

	mifare_desfire_disconnect (tags[0]);
	nfc_disconnect (device);
	free(tag_uid);
	destroy_keyvault (kv);
	freefare_free_tags(tags);

	return out_logentry;
}

void buy_python (char *global_priv_file, char *shop_priv_file, char *shop_name)
{
	uint8_t k_tag_crypted[128];

	nfc_device_t *device = NULL;
	int error = EXIT_SUCCESS;
	int res;
	size_t device_count;
	MifareTag *tags = NULL;
  nfc_device_desc_t devices;

	nfc_list_devices (&devices, 1, &device_count);
	if (!device_count)
		errx (EXIT_FAILURE, "No NFC device found.");

	device = nfc_connect (&devices);
	if (!device)
	{
		warnx ("nfc_connect() failed.");
		error = EXIT_FAILURE;
	}

	tags = freefare_get_tags (device);
	if (!tags) {
		nfc_disconnect (device);
		errx (EXIT_FAILURE, "Error listing tags.");
	}

	if (DESFIRE != freefare_get_tag_type (tags[0]))
	{
		printf ("Error: Tag is not a DESFire\n");
		nfc_disconnect (device);
		errx (EXIT_FAILURE, "Error listing tags.");
	}
	res = mifare_desfire_connect (tags[0]);

	MifareDESFireAID aid = mifare_desfire_aid_new (0x1);
	res = mifare_desfire_select_application(tags[0], aid);
	if (res < 0)
		errx (EXIT_FAILURE, "Application selection failed");
	free (aid);

	int read = 0;
	read = mifare_desfire_read_data (tags[0], 0x01, 0x00, 0x80, k_tag_crypted);
	if (read < 0)
		freefare_perror (tags[0], "Reading data");

	keyvault_t* kv = create_keyvault_new_empty ();
	int keylen = 0;
	RSA *private = load_key_from_file (global_priv_file, CRYPTO_PRIVATE);
	RSA *shop_private = load_key_from_file (shop_priv_file, CRYPTO_PRIVATE);
	keylen = RSA_private_decrypt (read, (unsigned char *) k_tag_crypted, kv->k, private, RSA_PKCS1_PADDING);
	if (keylen == 16)
		set_keytype_3DES (kv, CRYPTO_KEY_K);

	buy (tags[0], kv, shop_name, shop_private);


	(void) error;
	RSA_free (private);

	mifare_desfire_disconnect (tags[0]);
	nfc_disconnect (device);
	destroy_keyvault (kv);
	freefare_free_tags(tags);
}
