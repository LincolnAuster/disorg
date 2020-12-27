/* Reads filenames in from stdin. The files are then parsed into a
 * hash table, and aggregation is performed according to command
 * line arguments.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "conf.h"
#include "event.h"

#define INITIAL_BUFFER_SIZE 4

static void die(char *);

void buffer_append(char **, char, size_t *);
unsigned int match_int(int, const char *, const char *);
struct KeyValue * key_value_read(char *line);

/* output given string to stderr and exit */
static void
die(char *string) {
	fprintf(stderr, "%s", string);
	exit(1);
}

/* append given character to the buffer, resize if necessary */
void
buffer_append(char **buffer, char c, size_t *capacity)
{
	int text_len = strlen(*buffer);
	if (text_len + 2 > *capacity) {
		*capacity *= 2;
		char *new_buffer = realloc(*buffer, *capacity);
		*buffer = new_buffer;
	}

	(*buffer)[text_len++] = c;
	(*buffer)[text_len] = '\0';
}

/* scan a string for !KEY VALUE pairs, return KeyValue struct */
struct KeyValue *
key_value_read(char* line)
{
	char c;
	bool first_of_line, directive_key, directive_value;
	first_of_line = true;
	directive_key = directive_value = false;

	size_t key_size, val_size;
	key_size = val_size = INITIAL_BUFFER_SIZE;
	char *key = (char *) malloc(key_size * sizeof(char));
	char *val = (char *) malloc(val_size * sizeof(char));
	key[0] = val[0] = '\0';

	int len = strlen(line);
	for (int i = 0; i < len; i++) {
		c = line[i];

		if (c == ' ' && directive_key) {
			directive_key = false;
			directive_value = true;
			continue;
		}

		if (directive_key)
			buffer_append(&key, c, &key_size);
		else if (directive_value)
			buffer_append(&val, c, &val_size);

		if (first_of_line && c == '!')
			directive_key=true;
	}

	struct KeyValue *kv = malloc(sizeof(struct KeyValue));
	kv->key = key;
	kv->val = val;
	return kv;
}

int
main(int argc, char *argv[])
{
	/* collect configuration from environment variables into single config struct */
	struct Config conf;
	conf.date_format     = getenv("DATE_FORMAT");
	conf.time_format     = getenv("TIME_FORMAT");
	conf.week_start      = getenv("WEEK_START");
	conf.four_digit_year = getenv("FOUR_DIGIT_YEAR");

	/* Loop through all paths provided by stdin. If accessible,
	 * parse the event file and hash it by time.
	 */
	ssize_t len;
	size_t bufsize = 0;
	char* stdin_line = NULL;
	while ((len = getline(&stdin_line, &bufsize, stdin)) > 0) {
		stdin_line[--len] = '\0'; // strip newline from path

		FILE *event_file = fopen(stdin_line, "r");
		if (event_file == NULL) continue;

		Event* cur_event = event_new_empty(&conf);

		/* loop through a character at a time for !KEY VALUE items */
		char  *file_line    = NULL;
		size_t file_bufsize = 0;
		ssize_t len;
		struct KeyValue *pair = NULL;
		while ((len = getline(&file_line, &file_bufsize, event_file)) > 0) {
			file_line[--len] = '\0'; // strip newline
			pair = key_value_read(file_line);
			event_insert(cur_event, pair, &conf);
			file_line[0] = '\0';
		}

		fclose(event_file);
		event_display(cur_event);
		free(cur_event); /* maybe not later but for now */
	}

	return 0;
}
