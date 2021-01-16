/* scan a string for !KEY VALUE pairs, return KeyValue struct */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "global.h"

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
	int text_len = strlen(*buffer) + strlen(string);
	if (text_len > *capacity) {
		*capacity += strlen(string);
		char *new_buffer = realloc(*buffer, *capacity);
		*buffer = new_buffer;
	}

	for (int i = 0; i < strlen(string); i++)
		buffer_append(buffer, string[i], capacity);
}
