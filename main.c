#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <err.h>

#include <nfc/nfc.h>
#include <freefare.h>

#include "log.h"
#include "setup.h"

uint8_t k_m_1[16]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xee };
uint8_t k_m_2[16]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xee };
uint8_t k_w_1[16]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xee };
uint8_t k_tag[16]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xee };

wchar_t* our_name = L"Group5andwich";

int main (int argc, char** argv)
{
	nfc_device_t *device = NULL;
	int error = EXIT_SUCCESS;
	int res;
	size_t device_count;
	MifareTag *tags = NULL;
  nfc_device_desc_t devices[8];

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
	if (DESFIRE != freefare_get_tag_type (tags[0]))
	{
		printf ("Error: Tag is not a DESFire\n");
		nfc_disconnect (device);
		errx (EXIT_FAILURE, "Error listing tags.");
	}
	res = mifare_desfire_connect (tags[0]);
	/* We erase the card */
	erase_card (tags[0]);
	create_applications (tags[0]);
	create_files (tags[0]);

        /*for (int i = 0; (!error) && tags[i]; i++) {*/
            /*if (DESFIRE != freefare_get_tag_type (tags[i]))*/
                /*continue;*/

            /*int res;*/
            /*char *tag_uid = freefare_get_tag_uid (tags[i]);*/
						/*printf ("Found tag with UID 0x%s\n", tag_uid);*/

            /*res = mifare_desfire_connect (tags[i]);*/
            /*if (res < 0) {*/
                /*warnx ("Can't connect to Mifare DESFire target.");*/
                /*error = EXIT_FAILURE;*/
                /*break;*/
            /*}*/
						/*MifareDESFireKey key = mifare_desfire_des_key_new_with_version (initial_key);*/
						/*printf ("Authenticating to AID 0 ...\n");*/
						/*res = mifare_desfire_authenticate (tags[i], 0, key);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "Authentication on master application failed");*/
						/*mifare_desfire_key_free (key);*/

						/*printf ("Erasing card ...\n");*/
						/*res = mifare_desfire_format_picc (tags[i]);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "PICC format failed");*/


						/*[> Here we create the first application <]*/
						/*printf ("Creating Application #1...\n");*/
            /*MifareDESFireAID aid = mifare_desfire_aid_new (0x1);*/
						/*res = mifare_desfire_create_application (tags[i], aid, 0x0b, 0x2);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "Application creation failed, try erasing the card before retrying.");*/

						/*[> We select the first application <]*/
						/*res = mifare_desfire_select_application(tags[i], aid);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "Application selection failed");*/
						/*free (aid);*/

						/*[> We auth to AID 1 <]*/
						/*key = mifare_desfire_des_key_new_with_version (initial_key);*/
						/*printf ("Authenticating to AID 1...\n");*/
						/*res = mifare_desfire_authenticate (tags[i], 0, key);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "Authentication on application 1 failed");*/
						/*mifare_desfire_key_free (key);*/

						/*printf ("Creating std_data_file 0x01...\n");*/
						/*res = mifare_desfire_create_std_data_file(tags[i],0x01,0x03,0xE000,0x80);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "CreateStdDataFile failed");*/

						/*printf ("Creating std_data_file 0x02...\n");*/
						/*res = mifare_desfire_create_std_data_file(tags[i],0x02,0x03,0xE000,0x80);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "CreateStdDataFile failed");*/

						/*[> We select AID 0 again <]*/
						/*aid = mifare_desfire_aid_new (0x0);*/
						/*res = mifare_desfire_select_application(tags[i], aid);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "Application selection failed");*/
						/*free (aid);*/

						/*[> To create AID 2 we first need to auth to AID 0 again <]*/
						/*key = mifare_desfire_des_key_new_with_version (initial_key);*/
						/*printf ("Authenticating to AID 0 ...\n");*/
						/*res = mifare_desfire_authenticate (tags[i], 0, key);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "Authentication on application 1 failed");*/
						/*mifare_desfire_key_free (key);*/

						/*[> We now create the application <]*/
						/*printf ("Creating Application #2...\n");*/
            /*aid = mifare_desfire_aid_new (0x2);*/
						/*res = mifare_desfire_create_application (tags[i], aid, 0x0b, 0x2);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "Application creation failed, try erasing the card before retrying.");*/
						/*free (aid);*/

						/*[> We select AID 2 <]*/
						/*aid = mifare_desfire_aid_new (0x2);*/
						/*res = mifare_desfire_select_application(tags[i], aid);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "Application selection failed");*/
						/*free (aid);*/

						/*[> We auth to AID 2 <]*/
						/*key = mifare_desfire_des_key_new_with_version (initial_key);*/
						/*printf ("Authenticating to AID 2...\n");*/
						/*res = mifare_desfire_authenticate (tags[i], 0, key);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "Authentication on application 2 failed");*/
						/*mifare_desfire_key_free (key);*/

						/*printf ("Creating value_file 0x01...\n");*/
						/*res = mifare_desfire_create_value_file(tags[i], 0x01, 0x03, 0xE000, 0x0, 0x0fffffff, 0x0, 0x00);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "CreateValueFile failed");*/

						/*printf ("Creating cyclic_record_file 0x02...\n");*/
						/*res = mifare_desfire_create_cyclic_record_file(tags[i], 0x02, 0x03, 0E000, 0x0d, 0x0000a0);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "CreateValueFile failed");*/

						/*printf ("Increasing value of the counter by 1\n");*/
						/*res = mifare_desfire_credit(tags[i], 0x01, 0x1);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "Increasing value failed");*/

						/*res = mifare_desfire_commit_transaction (tags[i]);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "Commiting transaction failed");*/


						/*int32_t val = 0;*/
						/*res = mifare_desfire_get_value(tags[i], 0x01, &val);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "Getting value failed");*/
						/*printf ("Value is now %d\n", val);*/

						/*printf ("Increasing value of the counter by 1\n");*/
						/*res = mifare_desfire_credit(tags[i], 0x01, 0x1);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "Increasing value failed");*/

						/*res = mifare_desfire_commit_transaction (tags[i]);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "Commiting transaction failed");*/

						/*res = mifare_desfire_get_value(tags[i], 0x01, &val);*/
						/*if (res < 0)*/
							/*errx (EXIT_FAILURE, "Getting value failed");*/
						/*printf ("Value is now %d\n", val);*/



				/*}*/

	/*}*/


	(void) tags;
	(void) device;
	(void) res;

	freefare_free_tags (tags);
	nfc_disconnect (device);
	return error;
}
