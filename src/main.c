#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	char *name = NULL;
	if (argc == 2)
		name = argv[1];

	if ((name != NULL) && (strcmp(name, "--help") == 0)) {
		printf("Usage: %s [name]\n", argv[0]);
		return 0;
	}

	Config conf;
	conf.date_format     = getenv("DATE_FORMAT");
	conf.time_format     = getenv("TIME_FORMAT");
	conf.week_start      = getenv("WEEK_START");
	conf.four_digit_year = getenv("FOUR_DIGIT_YEAR");

	/* Loop through all paths provided by stdin. If accessible,
	 * parse the event file and hash it by time.
	 */
	ssize_t len;
	size_t bufsize;
	char* stdin_line = NULL;
	bufsize = 0;

	EventTree *et_root = NULL;
	int i = 0;

	while ((len = getline(&stdin_line, &bufsize, stdin)) > 0) {
		stdin_line[--len] = '\0'; /* strip newline from path */

		FILE *event_file = fopen(stdin_line, "r");
		if (event_file == NULL) continue;

		Event* cur_event = event_new_empty(&conf);
		event_fill_from_text(cur_event, event_file, &conf);

		printf("Parsing  : %s\n", cur_event->title);
		et_root = eventtree_insert(et_root, cur_event);
		fclose(event_file);
	}

	eventtree_in_order(et_root, &event_display);

	return 0;
}
