#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "wiki.h"

/* allocate and initialize a wiki entry */
Wiki *
wiki_new_empty()
{
	Wiki *w;
	w = malloc(sizeof(Wiki));
	w->title = NULL;
	w->desc  = NULL;
	w->text  = NULL;
	return w;
}

/* free all data of wiki entry w, assumes all fields are either NULL or
 * dynamically allocated.
 */
Wiki *
wiki_free(Wiki *w)
{
	if (w->title != NULL) free(w->title);
	if (w->desc  != NULL) free(w->desc);
	if (w->text  != NULL) free(w->text);
	free(w);
	return NULL;
}

/* printf an overview (metadata) of the provided wiki entry, excluding full
 * content
 */
void
wiki_disp(const Wiki *w)
{
	for (int i = 0; i < CLI_OUTPUT_LEN; i++) printf("-");
	printf("\n");

	printf("%-*sTITLE\n", CLI_OUTPUT_LEN - 5, w->title);
	if (w->desc != NULL)
		printf("%-*sDESCRIPTION\n", CLI_OUTPUT_LEN - 11, w->desc);
}

/* display a wiki entry verbosely, with all data and content */
void
wiki_vdisp(const Wiki *w)
{
	wiki_disp(w);
	printf("%s", w->text);
}
