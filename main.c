#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <err.h>
#include <string.h>

#include <nfc/nfc.h>
#include <freefare.h>

#include "log.h"
#include "setup.h"
#include "crypto.h"

uint8_t k_m_1[16]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xee };
uint8_t k_m_2[16]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xee };
uint8_t k_w_1[16]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xee };
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
	nfc_device_t *device = NULL;
	int error = EXIT_SUCCESS;
	int res;
	size_t device_count;
	MifareTag *tags = NULL;
  nfc_device_desc_t devices[8];

	generate_keys ();
	keyvault_t* kv = create_keyvault_new (k_m_1, k_w_1, k_m_2, k_tag);

	nfc_list_devices (devices, 8, &device_count);
	if (!device_count)
		errx (EXIT_FAILURE, "No NFC device found.");
	/*else*/
		/*printf ("Found %lu devices\n", device_count);*/

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
	strncat (filename, tag_uid, 255);
	write_keyvault_to_file (filename, kv);

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

	/* Test stuff a bit */
	/*int32_t val = 0;*/
	/*res = mifare_desfire_get_value(tags[0], 0x01, &val);*/
	/*if (res < 0)*/
		/*errx (EXIT_FAILURE, "Getting value failed");*/
	/*printf ("Value of counter is %d\n", val);*/

	/*printf ("Increasing value of the counter by 1\n");*/
	/*res = mifare_desfire_credit(tags[0], 0x01, 0x1);*/
	/*if (res < 0)*/
		/*errx (EXIT_FAILURE, "Increasing value failed");*/

	/*res = mifare_desfire_commit_transaction (tags[0]);*/
	/*if (res < 0)*/
		/*errx (EXIT_FAILURE, "Commiting transaction failed");*/

	/*res = mifare_desfire_get_value(tags[0], 0x01, &val);*/
	/*if (res < 0)*/
		/*errx (EXIT_FAILURE, "Getting value failed");*/
	/*printf ("Value of counter is now %d\n", val);*/


	(void) tags;
	(void) device;
	(void) res;
	printf ("Disconnecting ...\n");
	mifare_desfire_disconnect (tags[0]);

	freefare_free_tags (tags);
	nfc_disconnect (device);
	destroy_keyvault (kv);
	return error;
}
