typedef struct {
	char *title;
	char *desc;
	char *text;
} Wiki;

/* Hash table for wiki entries, hashed by name */
typedef struct {
	Wiki *arr;
} WikiTable;

Wiki *wiki_new_empty();
Wiki *wiki_free(Wiki *);
void  wiki_disp(const Wiki *);
void  wiki_vdisp(const Wiki *);
