#ifndef SETUP_H
#define SETUP_H

#include <freefare.h>

#include "crypto.h"

int erase_card (MifareTag tag);
int create_applications (MifareTag tag);
int create_files (MifareTag tag);
int setup_keys (MifareTag tag, keyvault_t *kv);
int write_encrypted_tag_key (MifareTag tag, keyvault_t *kv, RSA *global_public, RSA *shop_public, RSA *shop_private, size_t len);
#endif /* SETUP_H */
