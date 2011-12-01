#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <err.h>
#include <string.h>

#include <nfc/nfc.h>
#include <freefare.h>

#include <sandwich/log.h>
#include <sandwich/setup.h>
#include <sandwich/crypto.h>
#include <sandwich/shop.h>

uint8_t k_m_1[16]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t k_w_1[16]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xee };
uint8_t k_m_2[16]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xee };
uint8_t k_tag[16]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xee };

wchar_t* our_name = L"Group5andwich";

void generate_keys ()
{
	printf ("FIXME: These are not RANDOM!\nGenerating random keys ...\n");
	generate_random_key (k_m_1, 16);
	generate_random_key (k_w_1, 16);
	generate_random_key (k_m_2, 16);
	generate_random_key (k_tag, 16);
}

int main (int argc, char** argv)
{
	if (argc != 5)
	{
		printf ("Usage:\n%s keys/global_public.pem keys/global_private.pem keys/shop_public.pem keys/shop_private.pem\n", argv[0]);
		return EXIT_FAILURE;
	}

	RSA* global_public = load_key_from_file (argv[1], CRYPTO_PUBLIC);
	RSA* global_private = load_key_from_file (argv[2], CRYPTO_PRIVATE);
	RSA* shop_public = load_key_from_file (argv[3], CRYPTO_PUBLIC);
	RSA* shop_private = load_key_from_file (argv[4], CRYPTO_PRIVATE);

	nfc_device_t *device = NULL;
	int error = EXIT_SUCCESS;
	int res;
	size_t device_count;
	MifareTag *tags = NULL;
  nfc_device_desc_t devices[8];

	generate_keys ();
	keyvault_t* kv = create_keyvault_new (k_m_1, k_w_1, k_m_2, k_tag);

	set_keytype_non_crypted (kv, CRYPTO_KEY_KM1);
	set_keytype_non_crypted (kv, CRYPTO_KEY_KW1);
	set_keytype_non_crypted (kv, CRYPTO_KEY_KM2);
	set_keytype_non_crypted (kv, CRYPTO_KEY_K);

	set_keytype_DES (kv, CRYPTO_KEY_KM1);
	set_keytype_3DES (kv, CRYPTO_KEY_KW1);
	set_keytype_3DES (kv, CRYPTO_KEY_KM2);
	set_keytype_3DES (kv, CRYPTO_KEY_K);

	nfc_list_devices (devices, 1, &device_count);
	if (!device_count)
		errx (EXIT_FAILURE, "No NFC device found.");

	device = nfc_connect (devices);
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
	char filename[255];
	memset (filename, 0, 255);
	strncat (filename, "keyvaults/kv-", 255);
	strncat (filename, tag_uid, 230);
	write_keyvault_to_file (filename, kv);
	free(tag_uid);

	if (DESFIRE != freefare_get_tag_type (tags[0]))
	{
		printf ("Error: Tag is not a DESFire\n");
		nfc_disconnect (device);
		errx (EXIT_FAILURE, "Error listing tags.");
	}
	res = mifare_desfire_connect (tags[0]);

	erase_card (tags[0]);
	create_applications (tags[0]);
	create_files (tags[0]);
	setup_keys (tags[0], kv);
	write_encrypted_tag_key (tags[0], kv, global_public, shop_public, shop_private, 16);
	update_counter (tags[0], kv, 1);
	uint32_t val = read_counter (tags[0], kv);
	printf ("Counter value read from the tag is: %u\n", val);

  write_log (tags[0], kv, "group_1", val, shop_private);

	printf ("Disconnecting ...\n");
	mifare_desfire_disconnect (tags[0]);

	destroy_keyvault (kv);
	nfc_disconnect (device);
	freefare_free_tags (tags);
	RSA_free (global_public);
	RSA_free (global_private);
	RSA_free (shop_private);
	RSA_free (shop_public);
	(void) res;
	return error;
}
