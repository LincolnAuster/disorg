#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "conf.h"
#include "global.h"
#include "event.h"

/* read string in form `!KEY VALUE' to a KeyValue struct. If there is no key
 * specified, it is assumed to be MISC.
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
		key_len = strlen(key) + 1;
		val_len = strlen(line) - strlen(key);
		val = malloc(val_len);
		/* + 1 offsets the space trailing the !KEY */
		strcpy(val, line + key_len + 1);
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

/* exit the program on error, print error to stderr */
void
die(const char *s)
{
	fprintf(stderr, "%s", s);
	exit(1);
}

/* return an empty tm_struct (i.e., initialized to 0:0 0/0/1900) */
struct tm *
tm_empty()
{
	struct tm *t;
	t = malloc(sizeof(struct tm));
	t->tm_sec = 0;
	t->tm_min = 0;
	t->tm_hour = 0;
	t->tm_mday = 1;
	t->tm_mon  = 0;
	t->tm_year = 0;
	t->tm_yday = 0;
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
tm_dascii(const struct tm *t, const Config *c)
{
	char *time = malloc(11);
	sprintf(time, "%02d/%02d/%02d", t->tm_mday, t->tm_mon, t->tm_year);
	return time;
}
