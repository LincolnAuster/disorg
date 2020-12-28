#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#include "conf.h"
#include "event.h"

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

Event*    
event_new_empty(const struct Config *conf)
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
	e->hour        = 0;                                                                           
	e->minute      = 0;                                                                           
	e->day         = 0;                                                                           
	e->month       = 0;                                                                           
	e->year        = current_year;                                                                

	return e;    
}

/* loop through file line-by-line to fill fields of event */
void
event_fill_from_text(Event *e, FILE *f, const struct Config *c)
{
	char  *file_line    = NULL;
	size_t file_bufsize = 0;
	ssize_t len;
	struct KeyValue *pair = NULL;
	while ((len = getline(&file_line, &file_bufsize, f)) > 0) {
		file_line[--len] = '\0'; /* strip newline */
		pair = key_value_read(file_line);
		event_insert(e, pair, c);
	}
}

/* write the event to stdout with some pretty formatting ✨ */
void
event_display(Event *e)
{
	for (int i = 0; i < CLI_OUTPUT_LEN; i++) printf("-");
	printf("\n");

	printf("%-*sTITLE\n", CLI_OUTPUT_LEN - 5, e->title);
	printf("%-*sDESCRIPTION\n", CLI_OUTPUT_LEN - 11, e->description);
	
	printf("%02i", e->hour);
	printf(":%02i", e->minute);
	printf("\n");
	
	printf("%02i", e->day);
	printf("/%02i", e->month);
	printf("/%02i", e->year);
	printf("\n");
	
	printf("\n");
}

/* parse a key value pair into Event fields */                  
void                                                                         
event_insert(Event* e, struct KeyValue *k, const struct Config *conf)
{                                       
	if (strcmp(k->key, "TITLE") == 0) {
		e->title = (char *) malloc(strlen(k->val) * sizeof(char));
		strcpy(e->title, k->val);
	} else if (strcmp(k->key, "DESCRIPTION") == 0) {
		e->description = (char *) malloc(strlen(k->val) * sizeof(char));
		strcpy(e->description, k->val);
	} else if (strcmp(k->key, "TIME") == 0) {
		e->hour   = match_int(0, k->val, conf->time_format);
		e->minute = match_int(1, k->val, conf->time_format);
	} else if (strcmp(k->key, "DATE") == 0) {
		int day = match_int(0, k->val, conf->date_format);
		int month = match_int(1, k->val, conf->date_format);
		int year = match_int(2, k->val, conf->date_format);
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
}

/* returns 0 if equal, below if a < b and 1 if b > a */
int
event_compare(Event *a, Event *b)
{
	struct tm t;
	t.tm_year  = a->year;
	t.tm_mon   = a->month - 1;
	t.tm_mday  = a->day;
	t.tm_hour  = a->hour;
	t.tm_min   = a->minute;
	t.tm_sec   = 0;
	t.tm_isdst = 0;
	long int a_unix = mktime(&t);

	t.tm_year  = b->year;
	t.tm_mon   = b->month - 1;
	t.tm_mday  = b->day;
	t.tm_hour  = b->hour;
	t.tm_min   = b->minute;
	t.tm_sec   = 0;
	t.tm_isdst = 0;
	long int b_unix = mktime(&t);

	return a_unix - b_unix;
}