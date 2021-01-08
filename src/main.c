#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "conf.h"
#include "event.h"

char *target_arg(int, char **);
EventTree *build_tree(EventTree *, Config *conf, FILE *);

/* find a target event name in the args list, NULL if none */
char *
target_arg(int argc, char **argv)
{
	if (argc < 2) return NULL;

	char *target;
	for (int i = 1; i <= argc; i++) {
		if (argv[i][0] != '-') {
			target = malloc(strlen(argv[i] + 1));
			strcpy(target, argv[i]);
			return target;
		}
	}

	return NULL;
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
	char *target;
	EventTree *et_root;
	Config conf;

	conf.date_format     = getenv("DATE_FORMAT");
	conf.time_format     = getenv("TIME_FORMAT");
	conf.week_start      = getenv("WEEK_START");
	conf.four_digit_year = getenv("FOUR_DIGIT_YEAR");

	target = target_arg(argc, argv);

	if ((target != NULL) && (strcmp(target, "--help") == 0)) {
	 	printf("Usage: %s [name]\n", argv[0]);
	 	return 0;
	}

	et_root = NULL;
	et_root = build_tree(et_root, &conf, stdin);

	if (target != NULL)
		eventtree_if(et_root, target, &event_vdisp);
	else
		eventtree_in_order(et_root, &event_disp);

	free(target);
	et_root = eventtree_free(et_root);
	return 0;
}
