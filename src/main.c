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

static void die(char *);
unsigned int match_int(int, const char *, const char *);

/* output given string to stderr and exit */
static void
die(char *string) {
	fprintf(stderr, "%s", string);
	exit(1);
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

		event_fill_from_text(cur_event, event_file, &conf);
		fclose(event_file);
		event_display(cur_event);
		free(cur_event); /* maybe not later but for now */
	}

	return 0;
}
