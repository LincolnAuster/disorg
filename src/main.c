#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "conf.h"
#include "event.h"

static void die(char *string);
char *target_arg(int, char *[]);

/* output given string to stderr and exit */
static void
die(char *string) {
	fprintf(stderr, "%s", string);
	exit(1);
}

/* find a target event name in the args list */
char *
target_arg(int argc, char *argv[])
{
	if (argc > 1)
		return argv[1];
	else
		return NULL;
}

int
main(int argc, char *argv[])
{
	char *target = target_arg(argc, argv);
	if ((target != NULL) && (strcmp(target, "--help") == 0)) {
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
	while ((len = getline(&stdin_line, &bufsize, stdin)) > 0) {
		stdin_line[--len] = '\0'; /* strip newline from path */

		FILE *event_file = fopen(stdin_line, "r");
		if (event_file == NULL) continue;

		Event* cur_event = event_new_empty(&conf);
		event_fill_from_text(cur_event, event_file, &conf);

		if ((target != NULL) &&
		    (strcmp(target, cur_event->title) == 0)) {
			event_display(cur_event);
			printf("***\n%s***\n", cur_event->misc);
			et_root = eventtree_free(et_root);
			free(cur_event);
			free(stdin_line);
			return 0;
		}
		et_root = eventtree_insert(et_root, cur_event);
		fclose(event_file);
	}
	free(stdin_line);

	eventtree_in_order(et_root, &event_display);
	et_root = eventtree_free(et_root);
	return 0;
}
