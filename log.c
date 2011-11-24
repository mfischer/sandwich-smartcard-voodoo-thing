#include <stdlib.h>
#include <wchar.h>
#include <sys/time.h>
#include <time.h>

#include "log.h"

wchar_t *generate_log (wchar_t *shop_name)
{
	time_t cur = time (NULL);
	struct tm *tm_ptr = localtime ((const time_t *) &cur);
	wchar_t ret[200];
	/* FIXME get right length here */
	swprintf (ret, 200, L"%02d%02d%02d%02d%02d%04d%058S",
						-2000 + 1900 + tm_ptr->tm_year, tm_ptr->tm_mon, tm_ptr->tm_mday,
						 tm_ptr->tm_hour, tm_ptr->tm_min, 42, shop_name);
	wprintf (L"%S\n", ret);
	return NULL;
}
