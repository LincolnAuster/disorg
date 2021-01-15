/* scan a string for !KEY VALUE pairs, return KeyValue struct */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "global.h"

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
