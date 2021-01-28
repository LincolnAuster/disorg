#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "conf.h"
#include "util.h"
#include "event.h"

/* verify that an event is within the limit, if a field of limit is NULL
 * ignore. */
bool
matches_lim(const struct limit *l, const Event *e)
{
	if (l == NULL) return true;
	if (e == NULL) return false;

	if ((l->title_tar != NULL && e->title != NULL) &&
	     strcmp(e->title, l->title_tar) != 0)
		return false;
	if ((l->categ_tar != NULL && e->cat != NULL) &&
	     strcmp(e->cat, l->categ_tar) != 0)
		return false;
	if ((l->min != NULL && e->time != NULL) &&
		difftime(mktime(l->min), mktime(e->time)) < 0)
		return false;
	if ((l->max != NULL && e->time != NULL) &&
		difftime(mktime(l->max), mktime(e->time)) > 0)
		return false;
	return true;
}

/* initialize a new empty Event on 0/0/1900 year 0:0:0 */
Event *    
event_new_empty(const struct config *conf)
{
	Event *e = malloc(sizeof(Event));
	e->title        = NULL;
	e->cat          = NULL;
	e->description  = NULL;
	e->misc         = NULL;
	e->misc_cap     = 0;
	e->p            = 0;
	e->time         = tm_empty();
	e->short_disp   = false;

	return e;
}

/* initialize event with title TODAY on current date and time */
Event *
event_now(const struct config *conf)
{
	time_t t = time(NULL);
	struct tm *time = localtime(&t);

	Event *e = event_new_empty(conf);
	e->title = malloc(10);
	*(e->time) = *time;
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
	if (e->cat   != NULL)       free(e->cat);
	if (e->description != NULL) free(e->description);
	if (e->time != NULL)        free(e->time);
	if (e->misc != NULL)        free(e->misc);
	free(e);
	return NULL;
}

/* loop through file line-by-line to fill fields of event */
void
event_fill_from_text(Event *e, FILE *f, const struct config *c)
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
event_disp(const Event *e, const struct config *c)
{
	if (e->short_disp || c->wiki) event_sdisp(e, c);
	else               event_ndisp(e, c);
}

/* write the event to stdout with some pretty formatting ✨ */
void
event_ndisp(const Event *e, const struct config *c)
{
	char *time, *date;

	time = tm_tascii(e->time);
	date = tm_dascii(e->time, c);

	if (e->cat != NULL)
		printf("\033[38;5;%dm", buftocol(e->cat));

	for (int i = 0; i < c->col_width; i++) printf("-");

	/* bold title */
	printf("\n\033[1m");
	printf("%-*sTITLE\n", c->col_width - 5, e->title);
	printf(RESET_COLOR);
	if (e->cat != NULL)
		printf("\033[38;5;%dm", buftocol(e->cat));

	printf("%-*sDESCRIPTION\n", c->col_width - 11, e->description);
	printf("%-*sCATEGORY\n", c->col_width - 8, e->cat);
	
	printf("\033[%sm", c->pcolors[e->p]);

	printf("%s, (%d)\n", time, e->p);

	printf("%s\n", date);

	free(date);
	free(time);
	printf(RESET_COLOR);
}

/* display an event, but only on two lines (i.e., TODAY event) */
void
event_sdisp(const Event *e, const struct config *c)
{
	if (e->cat != NULL)
		printf("\033[38;5;%dm", buftocol(e->cat));
	for (int i = 0; i < c->col_width; i++) printf("-");
	printf("\n%s", e->title);
	if (e->cat != NULL) printf(" (%s)", e->cat);
	printf("%s\n", RESET_COLOR);
}

/* verbose display, display with miscellaneous field */
void
event_vdisp(const Event *e, const struct config *c) {
	event_disp(e, c);
	if (e->misc == NULL) return;
	print_long(e->misc, c->col_width);
}

/* parse a KeyValue into Event fields */                  
void
event_insert(Event* e, struct KeyValue *k, const struct config *conf)
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
event_insert_date(Event *e, const char *date, const struct config *conf)
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

/* insert category, free buffer if not NULL */
void
event_insert_category(Event *e, const char *p)
{
	if (e->cat == NULL) free(e->cat);
	e->cat = strdup(p);
}

/* insert a priority based on the PRIORITY_STR_* macros in global.h */
void
event_insert_priority(Event *e, char *text, const struct config *conf)
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
event_insert_misc(Event *e, char *text, const struct config *conf)
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
event_compare_time(const Event *a, const Event *b)
{
	time_t at = mktime(a->time);
	time_t bt = mktime(b->time);

	return (int) difftime(at, bt);
}

/* compare the events alphabetically, by category and then by name
 * case-insensitive */
int
event_compare_alpha(const Event *a, const Event *b)
{
	int cat = strcmp(a->cat, b->cat);
	return cat;
	if (cat == 0)
		return strcmp(a->title, b->title);
	else
		return cat;
}

/* return new empty event tree */
EventTree *
eventtree_new(void)
{
	EventTree *et = malloc(sizeof(EventTree));
	et->event = NULL;
	et->left  = NULL;
	et->right = NULL;
	/* by default, compare by time */
	et->cmp = &event_compare_time;
	return et;
}

/* initialize tree node */
EventTree *
eventtree_new_from_event(Event *e)
{
	EventTree *et = eventtree_new();
	et->event = e;
	return et;
}

/* insert an event into provided node */
EventTree *
eventtree_insert(EventTree *et, Event *e)
{
	/* two potential instances of base case, either tree itself is null
	 * or the event in the tree doesn't exist */
	if (et == NULL) return eventtree_new_from_event(e);
	if (et->event == NULL) {
		et->event = e;
		return et;
	}

	int cmp = et->cmp(et->event, e);
	if (cmp < 0)
		et->left  = eventtree_insert(et->left, e);
	else if (cmp >= 0)
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
eventtree_in_order(EventTree *et, const struct config *c, const struct limit *l,
		   void (*fun)(const Event *, const struct config *))
{
	if (et == NULL) return;
	eventtree_in_order(et->right, c, l, fun);
	if (matches_lim(l, et->event))
		fun(et->event, c);
	eventtree_in_order(et->left, c, l, fun);
}

/* perform function act on an event in the tree IFF cmp(et->event, c) returns
 * 0. Used to compare an event with config options.
 */
void
eventtree_if(EventTree *et, const struct config *c,
             int (*cmp)(const Event *, const struct config *),
             void (*act)(const Event *, const struct config *))
{
	if (et == NULL) return;
	eventtree_if(et->left, c, cmp, act);
	if (cmp(et->event, c) == 0) act(et->event, c);
	eventtree_if(et->right, c, cmp, act);
}

int
cat_cmp(const Event *e, const struct config *c)
{
	if (e->cat == NULL || c->tarcat == NULL) return -1;
	return strcmp(e->cat, c->tarcat);
}

int
tar_cmp(const Event *e, const struct config *c)
{
	return strcmp(e->title, c->target);
}
