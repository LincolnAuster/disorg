#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "global.h"
#include "conf.h"
#include "event.h"

#define USAGE "Usage: %s [w] [target] [--help]\n"                            \
              "  w      - query wiki pages instead of agenda\n"              \
	      "  target - show detailed view of event with title `target'\n" \
	      "  --help - show this help message\n"                          \
	      "If no arguments are provided, the agenda is enumerated.\n"    \
	      , argv[0]

struct cargs {
	char *target;
	bool wiki;
	bool help;
};

struct cargs *read_args(int, char **);
EventTree *build_tree(EventTree *, Config *conf, FILE *);

struct cargs *
read_args(int argc, char **argv) {
	struct cargs *c = malloc(sizeof(struct cargs));
	c->target = NULL;
	c->wiki = false;
	c->help = false;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "w") == 0)
			c->wiki = true;
		else if (strcmp(argv[i], "--help") == 0) {
			c->help = true;
		} else
			c->target = strdup(argv[i]);
	}

	return c;
}

/* build an EventTree from paths in a specified file. */
EventTree *
build_tree(EventTree *et_root, Config *conf, FILE *from) {
	char *in_line;
	FILE *event_file;
	in_line = NULL;

	ssize_t len;
	size_t bufsize;
	while ((len = getline(&in_line, &bufsize, from)) > 0) {
		in_line[--len] = '\0'; /* strip newline from path */

		event_file = fopen(in_line, "r");
		if (event_file == NULL) continue;

		Event* cur_event = event_new_empty(conf);
		event_fill_from_text(cur_event, event_file, conf);

		et_root = eventtree_insert(et_root, cur_event);
		fclose(event_file);
	}

	free(in_line);
	return et_root;
}

int
main(int argc, char **argv)
{
	struct cargs *args;
	EventTree *et_root;
	Config conf;

	conf.date_format     = getenv("DATE_FORMAT");
	conf.time_format     = getenv("TIME_FORMAT");
	conf.week_start      = getenv("WEEK_START");
	conf.four_digit_year = getenv("FOUR_DIGIT_YEAR");

	args = read_args(argc, argv);

	if (args->help) {
		printf(USAGE);
	 	return 0;
	}

	et_root = NULL;
	et_root = build_tree(et_root, &conf, stdin);

	if (args->target != NULL)
		eventtree_if(et_root, args->target, &event_vdisp);
	else
		eventtree_in_order(et_root, &event_disp);

	free(args->target);
	free(args);
	et_root = eventtree_free(et_root);
	return 0;
}
