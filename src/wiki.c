#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	/* i sold all my nonessential organs owo <3 */
	if (w->title != NULL) free(w->title);
	if (w->desc  != NULL) free(w->desc);
	if (w->text  != NULL) free(w->text);
	free(w);
	return NULL;
}

/* Grab information from wiki */
void
wiki_fill_from_text(Wiki *w, FILE *f)
{
	struct KeyValue *pair;
	char   *file_line    = NULL;
	size_t  file_bufsize, text_cap;
	ssize_t len;

	file_bufsize = 0;
	while ((len = getline(&file_line, &file_bufsize, f)) > 0) {
		file_line[--len] = '\0';
		
		text_cap = sizeof(w->text);
		pair = key_value_read(file_line);
		if (strcmp(pair->key, "TITLE") == 0)
			w->title = strdup(pair->val);
		else if (strcmp(pair->key, "DESCRIPTION") == 0)
			w->desc = strdup(pair->val);
		else if (strcmp(pair->key, "MISC") == 0)
			buffer_append_str(&(w->text), pair->val, &text_cap);

		free(pair->key);
		free(pair->val);
		free(pair);
	}
	free(file_line);
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
	printf("%s\n", w->text);
}

/* allocate a table */
WikiTable *
wikitable_new()
{
	WikiTable *t = malloc(sizeof(WikiTable));
	if (t == NULL) die("malloc failed - OOM");

	t->arr = malloc(sizeof(Wiki *) * HASH_SIZE);
	return t;
}

/* insert wiki entry into the hash table by title */
void
wikitable_insert(WikiTable *t, Wiki *w)
{
	unsigned int i = hash_str(w->title) % HASH_SIZE;
	t->arr[i] = w; /* TODO obviously stupid, i just cant think right now
			  so resolvhing collisions is future future me's problem. */
}
