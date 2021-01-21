#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "conf.h"
#include "global.h"
#include "event.h"

static struct cargs read_args(int, char **);
static EventTree *build_tree(EventTree *,
		      struct config *conf,
		      FILE *,
		      int (*)(Event *, Event *));

/* build an EventTree from paths in a specified file. */
static EventTree *
build_tree(EventTree *et_root,
           struct config *conf,
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
	EventTree *et_root;
	struct config conf;

	conf.date_format     = getenv("DATE_FORMAT");
	conf.time_format     = getenv("TIME_FORMAT");
	conf.four_digit_year = getenv("FOUR_DIGIT_YEAR");

	conf.today_color = getenv("TODAY_COLOR");
	conf.pcolors[0]  = getenv("PLOW_COLOR");
	conf.pcolors[1]  = getenv("PMED_COLOR");
	conf.pcolors[2]  = getenv("PHIGH_COLOR");

	conf.target = getenv("TARGET");
	conf.wiki   = conf_enabled(getenv("WIKI"));

	printf("target: %s\nwiki: %i\n", conf.target, conf.wiki);

	et_root = NULL;
	if (conf.wiki) {
		et_root = build_tree(et_root, &conf, stdin, event_compare_name);
	} else {
		et_root = build_tree(et_root, &conf, stdin, event_compare_time);
		Event *now = event_now(&conf);
		eventtree_insert(et_root, now, event_compare_time);
	}

	if (conf.target != NULL)
		eventtree_if(et_root, conf.target, &conf, event_vdisp);
	else
		eventtree_in_order(et_root, &conf, event_disp);

	et_root = eventtree_free(et_root);
	return 0;
}
