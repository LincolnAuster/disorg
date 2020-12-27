#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#include "conf.h"
#include "event.h"

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