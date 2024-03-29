#include "cache.h"

/*
 * Internal functions
 */
static void vreportf(const char *prefix, const char *err, va_list params)
{
	char msg[4096];
	vsnprintf(msg, sizeof(msg), err, params);
	fprintf(stderr, "%s%s\n", prefix, msg);
}

static NORETURN void die_routine(const char *err, va_list params){
  vreportf("Fatal: ", err, params);
  exit(128);
}

/*
 * Public functions
 */
void usagef(const char *err, ...)
{
	va_list params;

	va_start(params, err);
  vreportf("Usage: ", err, params);
  exit(129);
	va_end(params);
}

void usage(const char *err)
{
	usagef("%s", err);
}

void die(const char *err, ...)
{
	va_list params;

	va_start(params, err);
  die_routine(err, params);
	va_end(params);
}

void die_errno(const char *fmt, ...)
{
	va_list params;
	char fmt_with_err[1024];
	char str_error[256], *err;
	int i, j;

	err = strerror(errno);
	for (i = j = 0; err[i] && j < sizeof(str_error) - 1; ) {
		if ((str_error[j++] = err[i++]) != '%')
			continue;
		if (j < sizeof(str_error) - 1) {
			str_error[j++] = '%';
		} else {
			/* No room to double the '%', so we overwrite it with
			 * '\0' below */
			j--;
			break;
		}
	}
	str_error[j] = 0;
	snprintf(fmt_with_err, sizeof(fmt_with_err), "%s: %s", fmt, str_error);

	va_start(params, fmt);
	die_routine(fmt_with_err, params);
	va_end(params);
}

int error(const char *err, ...)
{
	va_list params;

	va_start(params, err);
  vreportf("Error: ", err, params);
	va_end(params);
	return -1;
}

void warning(const char *warn, ...)
{
	va_list params;

	va_start(params, warn);
  vreportf("Warning: ", warn, params);
	va_end(params);
}
