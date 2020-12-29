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

static void die(char *string);
void append_event(Event *, Event, size_t *);

/* output given string to stderr and exit */
static void
die(char *string) {
	fprintf(stderr, "%s", string);
	exit(1);
}

int
main(int argc, char *argv[])
{
	/* collect configuration from environment variables into a single
	 * config struct
	 */
	Config conf;
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

	EventTree *et_root = eventtree_new();
	int i = 0;
	
	while ((len = getline(&stdin_line, &bufsize, stdin)) > 0) {
		stdin_line[--len] = '\0'; /* strip newline from path */

		FILE *event_file = fopen(stdin_line, "r");
		if (event_file == NULL) continue;

		Event* cur_event = event_new_empty(&conf);
		eventtree_insert(et_root, cur_event);

		event_fill_from_text(cur_event, event_file, &conf);
		fclose(event_file);
		event_display(cur_event);
	}

	eventtree_in_order(et_root, &event_display);

	return 0;
}
