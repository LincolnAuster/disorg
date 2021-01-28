#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "conf.h"
#include "util.h"
#include "event.h"

int
main(int argc, char **argv)
{
	EventTree *et_root;
	struct config conf;
	struct limit lim;

	/* load config from environment */
	conf.date_format     = getenv("DATE_FORMAT");
	conf.time_format     = getenv("TIME_FORMAT");
	conf.four_digit_year = getenv("FOUR_DIGIT_YEAR");

	conf.today_color = getenv("TODAY_COLOR");
	conf.pcolors[0]  = getenv("PLOW_COLOR");
	conf.pcolors[1]  = getenv("PMED_COLOR");
	conf.pcolors[2]  = getenv("PHIGH_COLOR");

	if (!getenv("COL_WIDTH"))
		conf.col_width = 80;
	else
		conf.col_width = atoi(getenv("COL_WIDTH"));

	conf.target = strdup(getenv("TARGET"));
	conf.tarcat = strdup(getenv("CATEGORY"));
	if (strlen(conf.target) < 1) conf.target = NULL;
	if (strlen(conf.tarcat) < 1) conf.tarcat = NULL;
	conf.wiki = conf_enabled(getenv("WIKI"));

	/* allocate and configure the event tree */
	et_root = eventtree_new();
	if (conf.wiki)
		et_root->cmp = event_compare_alpha;
	else
		et_root->cmp = event_compare_time;

	/* build tree */
	et_root = eventtree_fromf(et_root, &conf, stdin);
	if (!conf.wiki)
		et_root = eventtree_insert(et_root, event_now(&conf));

	/* configure the limit */
	lim.min = NULL; /* TODO limit time to x days before, after, today */
	lim.max = NULL;
	lim.title_tar = conf.target;
	lim.categ_tar = conf.tarcat;

	/* output desired info */
	eventtree_in_order(et_root, &conf, &lim, event_disp);

	et_root = eventtree_free(et_root);
	if (conf.target != NULL) free(conf.target);
	if (conf.tarcat != NULL) free(conf.tarcat);
	return 0;
}
