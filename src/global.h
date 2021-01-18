#define CLI_OUTPUT_LEN 80
/* these numbers are all random guesses - i've tested nothing */
#define INITIAL_BUFFER_SIZE 12
#define HASH_SIZE           16

#define EVENT_EXT ".ev"
#define WIKI_EXT  ".wi"
#define WIKI_ARG "-W"

#define ACCENT_COLOR "\033[31;1;4m"
#define RESET_COLOR  "\033[0m"

struct KeyValue {
	char *key;
	char *val;
};

struct KeyValue *key_value_read(const char *);

void buffer_append(char **, const char, size_t *);
void buffer_append_str(char **, const char *, size_t *);
unsigned int hash_str(char *);

void die(const char *);
