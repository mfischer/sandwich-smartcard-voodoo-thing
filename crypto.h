#ifndef CRYPTO_H
#define CRYPTO_H

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/rand.h>
#include <inttypes.h>

#define CRYPTO_CUR_KEYVAULT_VERSION 0x01
#define CRYPTO_KEY_KM1 0x0
#define CRYPTO_KEY_KW1 0x1
#define CRYPTO_KEY_KM2 0x2
#define CRYPTO_KEY_K   0x3
#define CRYPTO_3DES 0x1
#define CRYPTO_DES  0x0
#define CRYPTO_CRYPTED 0x1
#define CRYPTO_PLAIN   0x0

/* This structure is used to store our keys.
 * The first 4 members are the corresponding keys.
 * The keytypes member is organized as follows:
 *
 * |MSB ........ LSB|
 * |K0K1K2K3C0C1C2C3|
 *
 * Where K0 is CRYPTO_3DES if k_m_1 is a 3DES key, CRYPTO_DES if it's a DES key.
 * The lower nibble stores whether the key in the keyvault is encrypted.
 * For example C0 is 1 if k_m_1 is encrypted.
 * The version parameter is used to store the version of the keyvault structure.
 */
struct keyvault
{
	uint8_t version;
	uint8_t k_m_1[16];
	uint8_t k_w_1[16];
	uint8_t k_m_2[16];
	uint8_t k[16];
	uint8_t keytypes;
};

typedef struct keyvault keyvault_t;

keyvault_t* create_keyvault_new (uint8_t *k_m_1, uint8_t *k_w_1, uint8_t *k_m_2, uint8_t *k);
keyvault_t* create_keyvault_new_empty ();
void destroy_keyvault (keyvault_t* kv);

void set_keytype_3DES (keyvault_t *kv, uint8_t keyno);
void set_keytype_DES (keyvault_t *kv, uint8_t keyno);

uint8_t get_keytype_3DES (keyvault_t *kv, uint8_t keyno);

void set_keytype_non_crypted (keyvault_t *kv, uint8_t keyno);
void set_keytype_crypted (keyvault_t *kv, uint8_t keyno);

int write_keyvault_to_file (const char* filename, keyvault_t* kv);
int init_keyvault_from_file (const char* filename, keyvault_t* kv);

X509* load_cert_from_file (const char *filename);
RSA* load_key_from_file (const char *filename, int private);
int generate_random_key (uint8_t* buf, int len);



#endif /* CRYPTO_H */
