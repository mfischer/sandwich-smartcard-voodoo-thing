#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <time.h>
#include <sys/time.h>

#include <nfc/nfc.h>
#include <freefare.h>

#include <sandwich/crypto.h>
#include <sandwich/shop.h>
#include <sandwich/log.h>

uint8_t k_m_1[16]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t k_tag[16]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t k_tag_crypted[128];

int main (int argc, char** argv)
{
	if (argc < 3)
	{
		printf ("Usage:\n%s keys/global_private.pem keys\n", argv[0]);
		return EXIT_FAILURE;
	}
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
	printf ("Found tag with UID: %s\n", tag_uid);

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
	else
		printf ("Read %d bytes of data ...\n", read);

	keyvault_t* kv = create_keyvault_new_empty ();
	int keylen = 0;
	RSA *private = load_key_from_file (argv[1], CRYPTO_PRIVATE);
	keylen = RSA_private_decrypt (read, (unsigned char *) k_tag_crypted, kv->k, private, RSA_PKCS1_PADDING);
	printf ("Decrypted %d bytes of K\n", keylen);
	if (keylen == 16)
		set_keytype_3DES (kv, CRYPTO_KEY_K);

	printf ("K is : ");
	for (size_t i = 0; i < (size_t) keylen; i++)
	{
		printf ("%02x", kv->k[i]);
	}
	printf ("\n");

	aid = mifare_desfire_aid_new (0x2);
	res = mifare_desfire_select_application(tags[0], aid);
	if (res < 0)
		errx (EXIT_FAILURE, "Application selection failed");
	free (aid);

	
	char logentry[LOG_MAX_LEN + 2 + 6];
	uint32_t counter = read_counter (tags[0], kv);
	uint32_t _counter = (counter >= LOG_MAX_ENTRIES) ? LOG_MAX_ENTRIES : counter;
	printf ("Read counter value of %u\n", counter);
	struct tm tm;
	char time[128];
	char shop_name[LOG_MAX_SHOP_LEN];
	uint32_t count;
	unsigned char signature[128];
	char shop_keyfile[255];

	for (size_t i = 0; i < _counter; i++)
	{
		read_log_entry (tags[0], kv, i, logentry);
		parse_logentry (logentry, &tm, shop_name, &count, signature);
		sprintf(shop_keyfile, "%s/public_keys/%s.pem", argv[2], shop_name);
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
		strftime (time, 128, "%x %X", &tm);
		printf ("Entry #%lu:\n\tTime: %s\n\tShop: \"%s\"\n\tCount: %u\n\tSignature ok? %s\n", i, time, shop_name, count, (sigok)? "Yes": "No");
	}

	(void) error;
	RSA_free (private);

	printf ("Disconnecting ...\n");
	mifare_desfire_disconnect (tags[0]);
	nfc_disconnect (device);
	free(tag_uid);
	destroy_keyvault (kv);
	freefare_free_tags(tags);
	return EXIT_SUCCESS;
}
