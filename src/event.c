#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "conf.h"
#include "global.h"
#include "event.h"

/* initialize a new empty Event on 0/0/current year 0:0:0 */
Event *    
event_new_empty(const Config *conf)
{
	Event *e = malloc(sizeof(Event));
	e->title        = NULL;
	e->description  = NULL;
	e->misc         = NULL;
	e->misc_cap     = 0;
	e->p            = 0;
	e->time         = tm_empty();
	e->short_disp   = false;

	return e;
}

/* initialize event with title Now on current date and time */
Event *
event_now(const Config *conf)
{
	time_t t = time(NULL);
	struct tm *time = localtime(&t);

	Event *e = event_new_empty(conf);
	e->title = malloc(10);
	*(e->time) = *time;
	e->time->tm_mon;
	e->short_disp  = true;

	sprintf(e->title, "\033[%smTODAY%s", conf->today_color, RESET_COLOR);

	return e;
}

/* free up all the fields, assumes dynamically allocated or null */
Event *
event_free(Event *e)
{
	if (e == NULL) return NULL;
	if (e->title != NULL)       free(e->title);
	if (e->description != NULL) free(e->description);
	if (e->time != NULL)        free(e->time);
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
event_disp(const Event *e, const Config *c)
{
	if (e->short_disp) event_sdisp(e);
	else               event_ndisp(e, c);
}

/* write the event to stdout with some pretty formatting ✨ */
void
event_ndisp(const Event *e, const Config *c)
{
	char *time, *date;

	time = tm_tascii(e->time);
	date = tm_dascii(e->time, c);
	for (int i = 0; i < CLI_OUTPUT_LEN; i++) printf("-");
	printf("\n");

	printf("%-*sTITLE\n", CLI_OUTPUT_LEN - 5, e->title);
	printf("%-*sDESCRIPTION\n", CLI_OUTPUT_LEN - 11, e->description);
	
	printf("\033[%sm", c->pcolors[e->p]);

	printf("%s, (%d)\n", time, e->p);

	printf("%s\n", date);

	free(date);
	free(time);
	printf(RESET_COLOR);
}

/* display an event, but only on two lines (i.e., TODAY event) */
void
event_sdisp(const Event *e)
{
	for (int i = 0; i < CLI_OUTPUT_LEN; i++) printf("-");
	printf("\n%s\n", e->title);
}

/* verbose display, display with miscellaneous field */
void
event_vdisp(const Event *e, const Config *c) {
	event_disp(e, c);
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
		e->time->tm_hour = match_int('H', k->val, conf->time_format);
		e->time->tm_min  = match_int('M', k->val, conf->time_format);
	} else if (strcmp(k->key, "DATE") == 0) {
		event_insert_date(e, k->val, conf);
	} else if (strcmp(k->key, "PRIORITY") == 0) {
		event_insert_priority(e, k->val, conf);
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

	if (year < 2000) year += 2000;
	year -= 1900;

	if (day == 0)
		day = e->time->tm_mday;
	if (month == 0)
		month = e->time->tm_mon;
	if (year == 0)
		year = e->time->tm_year;

	e->time->tm_mday  = day;
	e->time->tm_mon   = --month;
	e->time->tm_year  = year;
}

/* insert a priority based on the PRIORITY_STR_* macros in global.h */
void
event_insert_priority(Event *e, char *text, const Config *conf)
{
	if (strcmp(text, PRIORITY_STR_HIGH) == 0)
		e->p = 2;
	else if (strcmp(text, PRIORITY_STR_MID) == 0)
		e->p = 1;
	else if (strcmp(text, PRIORITY_STR_LOW) == 0)
		e->p = 0;
}

/* insert text into e->misc - set if empty, modify &(e->misc) otherwise */
void
event_insert_misc(Event *e, char *text, const Config *conf)
{
	char **addr;
	addr = &(e->misc);

	if (text[0] == '\0') return;
	if (e->misc == NULL) {
		e->misc_cap = strlen(text) + 2;
		e->misc = malloc(e->misc_cap);
		strcpy(e->misc, text);
	} else {
		buffer_append_str(addr, text, &(e->misc_cap));
	}

	buffer_append(addr, '\n', &(e->misc_cap));
}

/* returns 0 if equal, below if a < b and 1 if b > a */
int
event_compare_time(Event *a, Event *b)
{
	time_t at = mktime(a->time);
	time_t bt = mktime(b->time);

	return (int) difftime(at, bt);
}

/* strcmp the names of events, for alphabetical sorting */
int
event_compare_name(Event *a, Event *b)
{
	return strcmp(b->title, a->title);
}

/* return new empty event tree */
EventTree *
eventtree_new(void)
{
	EventTree *et = malloc(sizeof(EventTree));
	et->event = NULL;
	et->left  = NULL;
	et->right = NULL;
	return et;
}

/* initialize tree node */
EventTree *
eventtree_new_from_event(Event *e)
{
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
	else if (cmp >= 0)
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
eventtree_in_order(EventTree *et, const Config *c,
		   void (*fun)(const Event *, const Config *))
{
	if (et == NULL) return;
	eventtree_in_order(et->right, c, fun);
	fun(et->event, c);
	eventtree_in_order(et->left, c, fun);
}

/* Traverse the tree, call specified function if title matches provided
 * target.
 */
void
eventtree_if(EventTree *et, char *t_tgt, const Config *c,
             void (*fun)(const Event *, const Config *))
{
	if (et == NULL) return;
	if (strcmp(et->event->title, t_tgt) == 0) {
		fun(et->event, c);
		return;
	}

	eventtree_if(et->left, t_tgt, c, fun);
	eventtree_if(et->right, t_tgt, c, fun);
}
