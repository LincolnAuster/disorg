#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "global.h"
#include "conf.h"
#include "event.h"
#include "wiki.h"

struct cargs {
	char *target;
	bool wiki;
	bool help;
};

struct cargs *read_args(int, char **);
EventTree *build_data(EventTree *, Config *conf, FILE *);

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
build_tree(EventTree *et_root, Config *conf, FILE *from)
{
	char *path_l;
	FILE *event_file;
	ssize_t len;
	size_t bufsize;
	path_l = NULL;

	while ((len = getline(&path_l, &bufsize, from)) > 0) {
		path_l[--len] = '\0'; /* strip newline from path */
		/* verify file extension */
		if (strcmp(path_l + len - 3, EVENT_EXT) != 0) continue;

		event_file = fopen(path_l, "r");
		if (event_file == NULL) continue;

		Event* e = event_new_empty(conf);
		event_fill_from_text(e, event_file, conf);

		et_root = eventtree_insert(et_root, e);
		fclose(event_file);
	}

	free(path_l);
	return et_root;
}

/* build WikiTable from paths, similar to build_tree */
WikiTable *
build_table(WikiTable *t, FILE *from)
{
	char *path_l;
	FILE *wiki_file;
	size_t bufsize;
	ssize_t len;
	path_l = NULL;
	/* TODO this function correctly reads a wiki file, but
	 * - tags must be implemented (maybe by directory?)
	 * - hashing must be implmented
	 */

	while ((len = getline(&path_l, &bufsize, from)) > 0) {
		path_l[--len] = '\0'; /* strip newline */
		if (strcmp(path_l + len - 3, WIKI_EXT) != 0) continue;

		wiki_file = fopen(path_l, "r");
		if (wiki_file == NULL) continue;

		Wiki* w = wiki_new_empty();
		wiki_fill_from_text(w, wiki_file);
		wiki_vdisp(w);
	}

	return NULL;
}

int
main(int argc, char **argv)
{
	struct cargs *args;
	EventTree *et_root;
	WikiTable *wt;
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

	if (args->wiki) {
		build_table(NULL, stdin);
	} else {
		et_root = NULL;
		et_root = build_tree(et_root, &conf, stdin);

		if (args->target != NULL)
			eventtree_if(et_root, args->target, &event_vdisp);
		else
			eventtree_in_order(et_root, &event_disp);

		free(args->target);
		free(args);
		et_root = eventtree_free(et_root);
	}
	return 0;
}
