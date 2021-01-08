#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "conf.h"
#include "event.h"

/* scan a string for !KEY VALUE pairs, return KeyValue struct 
 * TODO this really must be shorter
 */
struct KeyValue *
key_value_read(char *line)
{
	size_t key_size, val_size;
	char *key, *val;
	bool first_of_line, directive_key, directive_value;
	char c;

	key_size = val_size = INITIAL_BUFFER_SIZE;
	key = malloc(key_size * sizeof(char));
	val = malloc(val_size * sizeof(char));
	key[0] = val[0] = '\0';
	first_of_line = true;
	directive_key = directive_value = false;

	int len = strlen(line);
	for (int i = 0; i < len; i++) {
		c = line[i];
		if (c == ' ' && directive_key) {
			directive_key = false;
			directive_value = true;
			continue;
		}
		if (first_of_line && c == '!') {
			directive_key = true;
			first_of_line = false;
			continue;
		}

		if (first_of_line && c != '!')
			directive_value = true;

		if (directive_key)
			buffer_append(&key, c, &key_size);
		if (directive_value)
			buffer_append(&val, c, &val_size);
	}

	if (key[0] == '\0') {
		size_t capacity = sizeof(key) * sizeof(key[0]);
		char *app = strdup("MISC");
		buffer_append_str(&key, app, &capacity);
		free(app);
	}

	struct KeyValue *kv = malloc(sizeof(struct KeyValue));
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

/* append given string to the buffer, resize if necessary */
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

Event*    
event_new_empty(const Config *conf)
{
        time_t s = time(NULL);

        struct tm *lt = localtime(&s);
        int current_year = lt->tm_year;
 
        if (conf_enabled(conf->four_digit_year))
		current_year += 1900;
        else
		current_year -= 100;

	Event *e = malloc(sizeof(Event));
	e->title       = NULL;
	e->description = NULL;
	e->misc        = NULL;
	e->hour        = 0;
	e->minute      = 0;
	e->day         = 0;
	e->month       = 0;
	e->year        = current_year;

	return e;
}

Event
*event_free(Event *e)
{
	if (e == NULL) return NULL;
	if (e->title != NULL)       free(e->title);
	if (e->description != NULL) free(e->description);
	if (e->misc != NULL)        free(e->misc);
	free(e);
	return NULL;
}

/* loop through file line-by-line to fill fields of event */
void
event_fill_from_text(Event *e, FILE *f, const Config *c)
{
	char   *file_line    = NULL;
	size_t  file_bufsize = 0;
	ssize_t len;
	while ((len = getline(&file_line, &file_bufsize, f)) > 0) {
		struct KeyValue *pair = NULL;
		file_line[--len] = '\0'; /* strip newline */
		pair = key_value_read(file_line);
		event_insert(e, pair, c);
		free(pair->val);
		free(pair->key);
		free(pair);
	}
	free(file_line);
}

/* write the event to stdout with some pretty formatting ✨ */
void
event_disp(Event *e)
{
	for (int i = 0; i < CLI_OUTPUT_LEN; i++) printf("-");
	printf("\n");

	printf("%-*sTITLE\n", CLI_OUTPUT_LEN - 5, e->title);
	printf("%-*sDESCRIPTION\n", CLI_OUTPUT_LEN - 11, e->description);
	
	printf("%02i", e->hour);
	printf(":%02i\n", e->minute);
	
	printf("%02i", e->day);
	printf("/%02i", e->month);
	printf("/%02i\n", e->year);
}

/* verbose display, display with miscellaneous field */
void
event_vdisp(Event *e) {
	event_disp(e);
	if (e->misc == NULL) return;
	printf("%s", e->misc);
}

/* parse a KeyValue into Event fields */                  
void
event_insert(Event* e, struct KeyValue *k, const Config *conf)
{
	if (strcmp(k->key, "TITLE") == 0) {
		e->title = malloc((strlen(k->val) + 1) * sizeof(char));
		strcpy(e->title, k->val);
	} else if (strcmp(k->key, "DESCRIPTION") == 0) {
		e->description = malloc((strlen(k->val) + 1) * sizeof(char));
		strcpy(e->description, k->val);
	} else if (strcmp(k->key, "TIME") == 0) {
		e->hour   = match_int('H', k->val, conf->time_format);
		e->minute = match_int('M', k->val, conf->time_format);
	} else if (strcmp(k->key, "DATE") == 0) {
		event_insert_date(e, k->val, conf);
	} else if (strcmp(k->key, "MISC") == 0) {
		event_insert_misc(e, k->val, conf);
	}
}

void
event_insert_date(Event *e, const char *date, const Config *conf)
{
	int day   = match_int('D', date, conf->date_format);
	int month = match_int('M', date, conf->date_format);
	int year  = match_int('Y', date, conf->date_format);
	if (day == 0)
		day = e->day;
	if (month == 0)
		month = e->month;
	if (year == 0)
		year = e->year;

	e->day   = day;
	e->month = month;
	e->year  = year;

	if (conf_enabled(conf->four_digit_year)) {
		e->year = make_four_digits(e->year);
	}
}

/* insert text into e->misc - set if empty, modify &(e->misc) otherwise */
void
event_insert_misc(Event *e, char *text, const Config *conf)
{
	size_t capacity;
	char **addr;

	capacity = sizeof(e->misc) / sizeof(char);
	addr     = &(e->misc);

	if (text[0] == '\0') return;
	if (e->misc == NULL) {
		e->misc = malloc((strlen(text) + 2) * sizeof(char));
		strcpy(e->misc, text);
		capacity += 2;
	} else {
		buffer_append_str(addr, text, &capacity);
	}

	capacity = sizeof(e->misc) / sizeof(char);
	buffer_append(addr, '\n', &capacity);
}

/* returns 0 if equal, below if a < b and 1 if b > a */
int
event_compare(Event *a, Event *b)
{
	struct tm t;
	t.tm_year  = a->year;
	t.tm_mon   = a->month;
	t.tm_mday  = a->day;
	t.tm_hour  = a->hour;
	t.tm_min   = a->minute;
	t.tm_sec   = 0;
	t.tm_isdst = 0;
	long int a_unix = mktime(&t);

	t.tm_year  = b->year;
	t.tm_mon   = b->month;
	t.tm_mday  = b->day;
	t.tm_hour  = b->hour;
	t.tm_min   = b->minute;
	t.tm_sec   = 0;
	t.tm_isdst = 0;
	long int b_unix = mktime(&t);

	return a_unix - b_unix;
}

/* return new empty event tree */
EventTree *
eventtree_new()
{
	EventTree *et = malloc(sizeof(EventTree));
	et->event = NULL;
	et->left  = NULL;
	et->right = NULL;
	return et;
}

EventTree *
eventtree_new_from_event(Event *e) {
	EventTree *et = malloc(sizeof(EventTree));
	et->event = e;
	et->left = NULL;
	et->right = NULL;
	return et;
}

/* insert an event into provided node */
EventTree *
eventtree_insert(EventTree *et, Event *e)
{
	if (et == NULL) return eventtree_new_from_event(e);
	int cmp = event_compare(et->event, e);

	if (cmp < 0)
		et->left  = eventtree_insert(et->left, e);
	else if (cmp > 0)
		et->right = eventtree_insert(et->right, e);

	return et;
}

/* recursively free a tree given root */
void *
eventtree_free(EventTree *et)
{
	if (et == NULL) return NULL;
	et->left  = eventtree_free(et->left);
	et->right = eventtree_free(et->right);
	et->event = event_free(et->event);
	free(et);
	return NULL;
}

/* perform an in-order traversal of the provided tree and
 * call the specified function on the values.
 */
void
eventtree_in_order(EventTree *et, void (*fun)(Event *))
{
	if (et == NULL) return;
	eventtree_in_order(et->right, fun);
	fun(et->event);
	eventtree_in_order(et->left, fun);
}

void
/* Traverse the tree, call specified function if title matches provided
 * target.
 */
eventtree_if(EventTree *et, char *t_tgt, void (*fun)(Event *))
{
	if (et == NULL) return;
	if (strcmp(et->event->title, t_tgt) == 0) {
		fun(et->event);
		return;
	}

	eventtree_if(et->left, t_tgt, fun);
	eventtree_if(et->right, t_tgt, fun);
}
