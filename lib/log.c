#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <inttypes.h>
#include <string.h>

#include <sandwich/log.h>

void generate_log (char *shop_name, uint32_t count, char* logentry)
{
	time_t cur = time (NULL);
	memset (logentry, 0, LOG_MAX_LEN);
	struct tm *tm_ptr = localtime ((const time_t *) &cur);
	snprintf (logentry, LOG_MAX_LEN, "%02d%02d%02d%02d%02d%04d%s",
			-2000 + 1900 + tm_ptr->tm_year, tm_ptr->tm_mon, tm_ptr->tm_mday,
			tm_ptr->tm_hour, tm_ptr->tm_min, count, shop_name);
}
