#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "conf.h"
#include "util.h"
#include "event.h"

/* read string in form `!KEY VALUE' to a dynamically allocated KeyValue struct.
 * If there is no key specified, it is assumed to be MISC.
 */
struct KeyValue *
key_value_read(const char *line)
{
	struct KeyValue *kv;
	char *key, *val;
	ssize_t key_len, val_len;

	if (line[0] == '!') {
		key = malloc(sizeof(line));
		sscanf(line, "!%s ", key);
		/* + 1 for null terminator, + 1 for trailing space */
		key_len = strlen(key) + 2;
		val_len = strlen(line) - strlen(key);
		val = malloc(val_len);
		strcpy(val, line + key_len);
	} else {
		key = strdup("MISC");
		val = strdup(line);
	}

	kv = malloc(sizeof(struct KeyValue));
	kv->key = key;
	kv->val = val;
	return kv;
}

/* append given character to the buffer, resize if necessary */
void
buffer_append(char **buffer, const char c, size_t *capacity)
{
	if (*buffer == NULL) {
		*buffer = malloc(sizeof(char));
		*buffer[0] = '\0';
	}

	int text_len = strlen(*buffer);
	if (text_len + 2 > *capacity) {
		*capacity *= 2;
		char *nb = realloc(*buffer, *capacity * sizeof(char));
		if (nb == NULL) {
			free(*buffer);
			*buffer = NULL;
			return;
		}
		*buffer = nb;
	}

	(*buffer)[text_len++] = c;
	(*buffer)[text_len] = '\0';
}

/* append given C-string to the buffer, resize if necessary */
void
buffer_append_str(char **buffer, const char *string, size_t *capacity)
{
	if (*buffer == NULL) {
		*buffer = strdup(string);
		*capacity = sizeof(string);
		return;
	}

	int text_len = strlen(*buffer) + strlen(string);
	if (text_len > *capacity) {
		*capacity += strlen(string);
		char *new_buffer = realloc(*buffer, *capacity);
		*buffer = new_buffer;
	}

	for (int i = 0; i < strlen(string); i++)
		buffer_append(buffer, string[i], capacity);
}

/* Procedurally acquire a color for a given string */
unsigned int
buftocol(const char *s)
{
	unsigned int c = 0;
	for (int i = 0; i < strlen(s); i++)
		c += s[i];
	/* limit to 16 colors to preserve terminal themeing */
	return 1 + c % 14;
}

/* find the parent dir given a / delimited filename - string returned 
 * is dynamically allocated */
char *
parent_dir(const char *p)
{
	char *buf;
	size_t start, end, len;
	start = end = 0;

	for (size_t i = strlen(p); i > 0; i--) {
		if (p[i] != '/') continue;
		if (end == 0) {
			end = i;
		} else if (start == 0) {
			start = ++i; break;
		}
	}

	len = end - start;
	buf = malloc(len + 1);
	strncpy(buf, p + start, len);
	buf[len] = '\0';

	return buf;
}

/* return an empty tm_struct (i.e., initialized to 0:0 0/0/1900) */
struct tm *
tm_empty(void)
{
	struct tm *t;
	t = malloc(sizeof(struct tm));
	t->tm_sec  = 0;
	t->tm_min  = 0;
	t->tm_hour = 0;
	t->tm_mday  = 1;
	t->tm_mon   = 0;
	t->tm_year  = 0;
	t->tm_yday  = 0;
	t->tm_isdst = 0;

	return t;
}

/* get a heap-allocated time (not date) string out of a config and struct tm */
char *
tm_tascii(const struct tm *t)
{
	char *time = malloc(7);
	sprintf(time, "%02d:%02d", t->tm_hour, t->tm_min);
	return time;
}

/* get a heap-allocated date (not time) string out of a config and struct tm */
char *
tm_dascii(const struct tm *t, const struct config *c)
{
	char  *mday, *mon, *year, *date;
	int mday_pos, mon_pos, year_pos, yearlen;
	char delim;

	delim = first_nonalpha(c->date_format);
	date = malloc(11);
	mday = malloc(3);
	mon  = malloc(3);
	year = malloc(5);

	sprintf(date, "  %c  %c    ", delim, delim);

	sprintf(mday, "%02d", t->tm_mday);
	sprintf(mon,  "%02d", t->tm_mon + 1);
	if (conf_enabled(c->four_digit_year))
		sprintf(year, "%04d", t->tm_year + 1900);
	else
		sprintf(year, "%02d", t->tm_year - 100);

	mday_pos = 3 * char_location('D', delim, c->date_format);
	mon_pos  = 3 * char_location('M', delim, c->date_format);
	year_pos = 3 * char_location('Y', delim, c->date_format);
	yearlen = 2 + conf_enabled(c->four_digit_year) * 2;

	strncpy(date + mday_pos, mday, 2);
	strncpy(date + mon_pos,  mon,  2);
	strncpy(date + year_pos, year, yearlen);

	free(mday);
	free(mon);
	free(year);
	return date;
}

/* print a string, wrapping lines at word breaks */
void
print_long(const char *s, size_t maxlen)
{
	for (size_t len = strlen(s); len > 0; ) {
		if (len <= maxlen) {
			printf("%s\n", s);
			break;
		}

		size_t end, next;
		for (end = maxlen; ; end--) {
			if (end == 0) {
				end  = maxlen;
				next = maxlen;
				break;
			}

			if (s[end] == ' ') {
				next = end + 1;
				break;
			}
		}

		printf("%.*s\n", (int) end, s);
		s += next;
		len -= next;
	}
}
