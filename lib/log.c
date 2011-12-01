#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <inttypes.h>
#include <string.h>

#include <sandwich/crypto.h>
#include <sandwich/log.h>

void generate_log (char *shop_name, uint32_t count, char* logentry, RSA *shop_private)
{
	int res;
	time_t cur = time (NULL);
	memset (logentry, 0, LOG_MAX_LEN);
	struct tm *tm_ptr = localtime ((const time_t *) &cur);
	snprintf (logentry, LOG_MAX_LEN, "%02d%02d%02d%02d%02d%04u%s",
			-2000 + 1900 + tm_ptr->tm_year, tm_ptr->tm_mon, tm_ptr->tm_mday,
			tm_ptr->tm_hour, tm_ptr->tm_min, count, shop_name);

	unsigned int digestlen = 72;
	unsigned int siglen = RSA_size (shop_private);
	unsigned char *digest = digest_message ((uint8_t *) logentry, &digestlen);
	res = RSA_sign (NID_sha1, digest, digestlen,  (unsigned char *) logentry + 72, &siglen , shop_private);
	if (res <= 0)
		fprintf (stderr, "Something went wrong while signing\n");
}

void parse_logentry (char *logentry, struct tm *time, char *shop_name, uint32_t *count, unsigned char *signature)
{
	sscanf (logentry, "%02d%02d%02d%02d%02d", &(time->tm_year), &(time->tm_mon), &(time->tm_mday), &(time->tm_hour), &(time->tm_min));
	time->tm_year += 2000 - 1900;
	time->tm_isdst = -1;
	time->tm_mon--;
	memcpy (shop_name, logentry + 14, 58);
	sscanf (logentry + 10, "%4u", count);
	memcpy (signature, logentry+72, 128);
}
