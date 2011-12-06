#ifndef LOG_H
#define LOG_H

#include <inttypes.h>
#include <time.h>

#include <sandwich/crypto.h>

#define LOG_MAX_LEN 200
#define LOG_PAYLOAD_LEN 72
#define LOG_MAX_SHOP_LEN 58
#define LOG_MAX_ENTRIES 10
void generate_log (char *shop_name, uint32_t count, char* logentry, RSA *shop_private);
void parse_logentry (char *logentry, struct tm *time, char *shop_name, uint32_t *count, unsigned char *signature);

#endif /* LOG_H */
