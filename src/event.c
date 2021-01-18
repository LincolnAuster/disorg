#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "global.h"
#include "conf.h"
#include "event.h"

/* initialize a new empty Event on 0/0/current year 0:0:0 */
Event *    
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
	e->short_disp  = false;

	return e;
}

/* initialize event with title Now on current date and time */
Event *
event_now(const Config *conf)
{
        time_t s = time(NULL);

        struct tm *lt = localtime(&s);
        int current_year  = lt->tm_year;
	int current_month = lt->tm_mon;
	int current_day   = lt->tm_mday;
	int current_hour  = lt->tm_hour;
	int current_min   = lt->tm_min;
 
        if (conf_enabled(conf->four_digit_year))
		current_year += 1900;
        else
		current_year -= 100;

	Event *e = malloc(sizeof(Event));
	e->title = malloc(10);
	sprintf(e->title, "%sTODAY%s", ACCENT_COLOR, RESET_COLOR);
	e->description = NULL;
	e->misc        = NULL;
	e->hour        = current_hour;
	e->minute      = current_min;
	e->day         = current_day;
	e->month       = current_month;
	e->year        = current_year;
	e->short_disp  = true;

	return e;

}

/* free up all the fields, assumes dynamically allocated or null */
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
	struct KeyValue *pair;
	char   *file_line    = NULL;
	size_t  file_bufsize = 0;
	ssize_t len;

	while ((len = getline(&file_line, &file_bufsize, f)) > 0) {
		file_line[--len] = '\0'; /* strip newline */
		if (len < 2) continue;   /* ignore empty lines */

		pair = key_value_read(file_line);
		event_insert(e, pair, c);
		free(pair->val);
		free(pair->key);
		free(pair);
	}
	free(file_line);
}

/* write the event to stdout, decide on type of formatting */
void
event_disp(const Event *e)
{
	if (e->short_disp) event_sdisp(e);
	else               event_ndisp(e);
}

/* write the event to stdout with some pretty formatting ✨ */
void
event_ndisp(const Event *e)
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

/* display an event, but only on two lines (i.e., TODAY event */
void
event_sdisp(const Event *e)
{
	for (int i = 0; i < CLI_OUTPUT_LEN; i++) printf("-");
	printf("\n%s\n", e->title);
}

/* verbose display, display with miscellaneous field */
void
event_vdisp(const Event *e) {
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

/* insert provided date string into e->date */
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
	} else if (e->year > 2000)
		e->year -= 2000;
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
event_compare_time(Event *a, Event *b)
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

/* strcmp the names of events, for alphabetical sorting */
int
event_compare_name(Event *a, Event *b)
{
	return strcmp(a->title, b->title);
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

/* initialize tree node */
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
eventtree_insert(EventTree *et, Event *e, int (*comp)(Event *, Event *))
{
	if (et == NULL) return eventtree_new_from_event(e);
	int cmp = comp(et->event, e);

	if (cmp < 0)
		et->left  = eventtree_insert(et->left, e, comp);
	else if (cmp > 0)
		et->right = eventtree_insert(et->right, e, comp);

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
eventtree_in_order(EventTree *et, void (*fun)(const Event *))
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
eventtree_if(EventTree *et, char *t_tgt, void (*fun)(const Event *))
{
	if (et == NULL) return;
	if (strcmp(et->event->title, t_tgt) == 0) {
		fun(et->event);
		return;
	}

	eventtree_if(et->left, t_tgt, fun);
	eventtree_if(et->right, t_tgt, fun);
}
