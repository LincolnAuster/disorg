#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "global.h"
#include "conf.h"
#include "event.h"

struct cargs {
	char *target;
	bool wiki;
};

struct cargs *read_args(int, char **);
EventTree *build_tree(EventTree *,
		      Config *conf,
		      FILE *,
		      int (*)(Event *, Event *));

struct cargs *
read_args(int argc, char **argv) {
	struct cargs *c = malloc(sizeof(struct cargs));
	c->target = NULL;
	c->wiki = false;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], WIKI_ARG) == 0) {
			c->wiki = true;
		} else {
			if (c->target != NULL) free(c->target);
			c->target = strdup(argv[i]);
		}
	}

	return c;
}

/* build an EventTree from paths in a specified file. */
EventTree *
build_tree(EventTree *et_root,
           Config *conf,
	   FILE *from,
	   int (*comp)(Event *, Event *))
{
	char *path_l;
	FILE *event_file;
	ssize_t len;
	size_t bufsize;

	path_l  = NULL;
	bufsize = 0;
	while ((len = getline(&path_l, &bufsize, from)) > 0) {
		path_l[--len] = '\0'; /* strip newline from path */

		event_file = fopen(path_l, "r");
		if (event_file == NULL) continue;

		Event* e = event_new_empty(conf);
		event_fill_from_text(e, event_file, conf);

		et_root = eventtree_insert(et_root, e, comp);
		fclose(event_file);
	}

	free(path_l);
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

	et_root = NULL;
	if (args->wiki) {
		et_root = build_tree(et_root, &conf, stdin, event_compare_name);
	} else {
		et_root = build_tree(et_root, &conf, stdin, event_compare_time);
		Event *now = event_now(&conf);
		eventtree_insert(et_root, now, event_compare_time);
	}

	if (args->target != NULL)
		eventtree_if(et_root, args->target, event_vdisp);
	else
		eventtree_in_order(et_root, event_disp);

	free(args->target);
	free(args);
	et_root = eventtree_free(et_root);
	return 0;
}
