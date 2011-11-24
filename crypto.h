#ifndef CRYPTO_H
#define CRYPTO_H

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/rand.h>
#include <inttypes.h>

struct keyvault
{
	uint8_t k_m_1[16];
	uint8_t k_w_1[16];
	uint8_t k_m_2[16];
	uint8_t k[16];
};

typedef struct keyvault keyvault_t;

keyvault_t* create_keyvault_new (uint8_t *k_m_1, uint8_t *k_w_1, uint8_t *k_m_2, uint8_t *k);
void destroy_keyvault (keyvault_t* kv);
int write_keyvault_to_file (const char* filename, keyvault_t* kv);
int init_keyvault_from_file (const char* filename, keyvault_t* kv);

X509* load_cert_from_file (const char *filename);
RSA* load_key_from_file (const char *filename, int private);
int generate_random_key (uint8_t* buf, int len);



#endif /* CRYPTO_H */
