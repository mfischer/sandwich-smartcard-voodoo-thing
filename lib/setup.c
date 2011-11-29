#include <stdlib.h>
#include <stdio.h>
#include <err.h>

#include <openssl/err.h>

#include <sandwich/setup.h>
#include <sandwich/crypto.h>


uint8_t initial_key[8]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

int write_encrypted_tag_key (MifareTag tag, keyvault_t *kv, RSA *global_public, RSA *shop_public, RSA *shop_private, size_t len)
{
	int res = 0;

	uint8_t *crypted = malloc (RSA_size(global_public));
	res = RSA_public_encrypt (16, (unsigned char*) kv->k, (unsigned char*) crypted, global_public, RSA_PKCS1_PADDING);
	if (res < 0)
		fprintf (stderr, "Something went wrong while ciphering\n");
	printf ("Encrypted key has length %d\n", RSA_size(global_public));

	unsigned int siglen = RSA_size (shop_private);
	unsigned int digestlen = RSA_size(global_public);
	unsigned char *digest = digest_message (crypted, &digestlen);
	uint8_t *signature = malloc (siglen);
	res = RSA_sign (NID_sha1, digest, digestlen,  (unsigned char *) signature, &siglen , shop_private);
	if (res <= 0)
		fprintf (stderr, "Something went wrong while signing\n");

	res = RSA_verify (NID_sha1, (unsigned char*) digest, digestlen, (unsigned char *) signature, siglen, shop_public);
	if (res <= 0)
		fprintf (stderr, "Something went wrong while signing, can't verify the thing with our pubkey\n");



	MifareDESFireAID aid = mifare_desfire_aid_new (0x1);
	res = mifare_desfire_select_application(tag, aid);
	if (res < 0)
		errx (EXIT_FAILURE, "Application selection failed");
	free (aid);

	MifareDESFireKey key = mifare_desfire_3des_key_new_with_version (kv->k_w_1);
	mifare_desfire_key_set_version (key, 0x01);
	res = mifare_desfire_authenticate (tag, 1, key);
	if (res < 0)
		freefare_perror(tag, "Authentication to application #1 failed");
	mifare_desfire_key_free (key);

	ssize_t written = mifare_desfire_write_data (tag, 0x01, 0x0, 0x80, crypted);
	if (written < 0)
		freefare_perror(tag, "Writing data to tag");
	else
		printf ("Wrote %ld bytes E(K) to card ...\n", written);

	written = mifare_desfire_write_data (tag, 0x02, 0x0, 0x80, signature);
	if (written < 0)
		freefare_perror(tag, "Writing data to tag");
	else
		printf ("Wrote %ld bytes Sign(E(K)) to card ...\n", written);

	free (crypted);
	free (signature);
	free (digest);
	return res;
}

int create_applications (MifareTag tag)
{
	int res = 0;
	MifareDESFireKey key = mifare_desfire_des_key_new_with_version (initial_key);
	res = mifare_desfire_authenticate (tag, 0, key);
	if (res < 0)
		errx (EXIT_FAILURE, "Authentication to master application failed");
	mifare_desfire_key_free (key);

	printf ("Creating Application #1...\n");
	MifareDESFireAID aid = mifare_desfire_aid_new (0x1);
	res = mifare_desfire_create_application (tag, aid, 0x0f, 0x2);
	if (res < 0)
		errx (EXIT_FAILURE, "Application #1 creation failed, try erasing the card before retrying.");
	free(aid);

	printf ("Creating Application #2...\n");
	aid = mifare_desfire_aid_new (0x2);
	res = mifare_desfire_create_application (tag, aid, 0x0f, 0x2);
	if (res < 0)
		errx (EXIT_FAILURE, "Application #2 creation failed, try erasing the card before retrying.");
	free(aid);

	return res;
}

int erase_card (MifareTag tag)
{
	int res = 0;
	MifareDESFireKey key = mifare_desfire_des_key_new_with_version (initial_key);
	res = mifare_desfire_authenticate (tag, 0, key);
	if (res < 0)
		errx (EXIT_FAILURE, "Authentication on master application failed");
	mifare_desfire_key_free (key);

	printf ("Erasing card ...\n");
	res = mifare_desfire_format_picc (tag);
	if (res < 0)
		errx (EXIT_FAILURE, "PICC format failed");

	return res;
}

int create_files (MifareTag tag)
{
	int res = 0;
	MifareDESFireAID aid = mifare_desfire_aid_new (0x1);
	res = mifare_desfire_select_application(tag, aid);
	if (res < 0)
		errx (EXIT_FAILURE, "Application selection failed");
	free (aid);

	MifareDESFireKey key = mifare_desfire_des_key_new_with_version (initial_key);
	res = mifare_desfire_authenticate (tag, 0, key);
	if (res < 0)
	errx (EXIT_FAILURE, "Authentication to application 1 failed");
	mifare_desfire_key_free (key);

	/* FIXME: Check the accessrights ... */
	printf ("Creating std_data_file 0x01...\n");
	res = mifare_desfire_create_std_data_file(tag, 0x01,0x03,0xe1ff,0x80);
	if (res < 0)
	errx (EXIT_FAILURE, "CreateStdDataFile failed");

	/* FIXME: Check the accessrights (See section 3.3 of the datasheet)... */
	printf ("Creating std_data_file 0x02...\n");
	res = mifare_desfire_create_std_data_file(tag, 0x02,0x03,0xe111,0x80);
	if (res < 0)
		errx (EXIT_FAILURE, "CreateStdDataFile failed");

	aid = mifare_desfire_aid_new (0x2);
	res = mifare_desfire_select_application(tag, aid);
	if (res < 0)
		errx (EXIT_FAILURE, "Application selection failed");
	free (aid);

	key = mifare_desfire_des_key_new_with_version (initial_key);
	res = mifare_desfire_authenticate (tag, 0, key);
	if (res < 0)
		errx (EXIT_FAILURE, "Authentication to application 2 failed");
	mifare_desfire_key_free (key);

	/* FIXME: Check the accessrights ... */
	printf ("Creating std_data_file 0x01...\n");
	res = mifare_desfire_create_std_data_file(tag, 0x01,0x03, 0xe110, 0x20);
	if (res < 0)
		errx (EXIT_FAILURE, "CreateValueFile failed");

	/* FIXME: Check the accessrights ... */
	printf ("Creating std_data_file 0x02...\n");
	res = mifare_desfire_create_std_data_file(tag, 0x02,0x03, 0xf110, 0x7e0);
	if (res < 0)
		errx (EXIT_FAILURE, "CreateLinearRecordFile failed");

	return EXIT_SUCCESS;
}

int setup_keys (MifareTag tag, keyvault_t *kv)
{
	int res = 0;
	MifareDESFireAID aid = mifare_desfire_aid_new (0x1);
	res = mifare_desfire_select_application(tag, aid);
	if (res < 0)
		errx (EXIT_FAILURE, "Application selection failed");
	free (aid);

	MifareDESFireKey old_key = mifare_desfire_des_key_new_with_version (initial_key);
	res = mifare_desfire_authenticate (tag, 0, old_key);
	if (res < 0)
		errx (EXIT_FAILURE, "Authentication to application 1 failed");

	MifareDESFireKey new_key;
	if (get_keytype_3DES (kv, CRYPTO_KEY_KM1))
	{
		printf ("Setting 3DES key K_M1 ...\n");
		new_key = mifare_desfire_3des_key_new_with_version (kv->k_m_1);
	}
	else
	{
		printf ("Setting DES key K_M1 ...\n");
		new_key = mifare_desfire_des_key_new_with_version (kv->k_m_1);
	}

	res = mifare_desfire_change_key (tag, 0x00, new_key, old_key);
	if (res < 0)
		freefare_perror (tag, "Change key 0x00");

	res = mifare_desfire_authenticate (tag, 0, new_key);
	if (res < 0)
		freefare_perror (tag, "Reauthing after changing key 0x00");

	mifare_desfire_key_free (new_key);

	if (get_keytype_3DES (kv, CRYPTO_KEY_KW1))
	{
		printf ("Setting 3DES key K_W1 ...\n");
		new_key = mifare_desfire_3des_key_new_with_version (kv->k_w_1);
	}
	else
	{
		printf ("Setting DES key K_W1 ...\n");
		new_key = mifare_desfire_des_key_new_with_version (kv->k_w_1);
	}
	res = mifare_desfire_change_key (tag, 0x01, new_key, old_key);
	if (res < 0)
		freefare_perror (tag, "Change key 0x01");
	mifare_desfire_key_free (new_key);

	/* AID 2 ... */

	aid = mifare_desfire_aid_new (0x2);
	res = mifare_desfire_select_application(tag, aid);
	if (res < 0)
		errx (EXIT_FAILURE, "Application selection failed");
	free (aid);

	res = mifare_desfire_authenticate (tag, 0, old_key);
	if (res < 0)
		errx (EXIT_FAILURE, "Authentication to application 2 failed");

	if (get_keytype_3DES (kv, CRYPTO_KEY_KM2))
	{
		printf ("Setting 3DES key K_M2 ...\n");
		new_key = mifare_desfire_3des_key_new_with_version (kv->k_m_2);
	}
	else
	{
		printf ("Setting DES key K_M2 ...\n");
		new_key = mifare_desfire_des_key_new_with_version (kv->k_m_2);
	}
	res = mifare_desfire_change_key (tag, 0x00, new_key, old_key);
	if (res < 0)
		freefare_perror (tag, "Change key 0x00");

	res = mifare_desfire_authenticate (tag, 0, new_key);
	if (res < 0)
		freefare_perror (tag, "Reauthing after changing key 0x00");

	mifare_desfire_key_free (new_key);

	if (get_keytype_3DES (kv, CRYPTO_KEY_K))
	{
		printf ("Setting 3DES key K ...\n");
		new_key = mifare_desfire_3des_key_new_with_version (kv->k);
	}
	else
	{
		printf ("Setting DES key K ...\n");
		new_key = mifare_desfire_des_key_new_with_version (kv->k);
	}
	res = mifare_desfire_change_key (tag, 0x01, new_key, old_key);
	if (res < 0)
		freefare_perror (tag, "Change key 0x01");
	mifare_desfire_key_free (new_key);
	mifare_desfire_key_free (old_key);

	return EXIT_SUCCESS;
}
