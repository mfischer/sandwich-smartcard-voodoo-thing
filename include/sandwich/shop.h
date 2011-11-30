#ifndef SHOP_H
#define SHOP_H

#include <inttypes.h>
#include <freefare.h>

#include <sandwich/crypto.h>

void update_counter (MifareTag tg, const keyvault_t *kv, uint32_t value);
uint32_t read_counter (MifareTag tg, const keyvault_t *kv);
void write_log (MifareTag tag, const keyvault_t *kv, uint8_t *data, uint32_t count, RSA *shop_private);

#endif /* SHOP_H */
