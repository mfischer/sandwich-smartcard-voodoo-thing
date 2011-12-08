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
	if (argc != 3)
	{
		printf ("Usage:\n%s keys/global_private.pem keys/shop_private.pem\n", argv[0]);
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

	keyvault_t* kv = create_keyvault_new_empty ();
	int keylen = 0;
	RSA *private = load_key_from_file (argv[1], CRYPTO_PRIVATE);
	RSA *shop_private = load_key_from_file (argv[2], CRYPTO_PRIVATE);
	keylen = RSA_private_decrypt (read, (unsigned char *) k_tag_crypted, kv->k, private, RSA_PKCS1_PADDING);
	if (keylen == 16)
		set_keytype_3DES (kv, CRYPTO_KEY_K);

	buy (tags[0], kv, "group_1", shop_private);


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
