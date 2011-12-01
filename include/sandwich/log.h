#ifndef LOG_H
#define LOG_H

#include <inttypes.h>
#include <time.h>

#include <sandwich/crypto.h>

#define LOG_MAX_LEN 200
void generate_log (char *shop_name, uint32_t count, char* logentry, RSA *shop_private);
void parse_logentry (char *logentry, struct tm *time, char *shop_name, uint32_t *count, unsigned char *signature);

#endif /* LOG_H */
