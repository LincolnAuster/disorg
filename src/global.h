#define CLI_OUTPUT_LEN 80
/* these numbers are all random guesses - i've tested nothing */
#define INITIAL_BUFFER_SIZE 12
#define HASH_TABLE_SIZE 16

#define EVENT_EXT ".ev"
#define WIKI_EXT  ".wi"

#define USAGE "Usage: %s [w] [target] [--help]\n"                            \
              "  w      - query wiki pages instead of agenda\n"              \
              "  target - show detailed view of event with title `target'\n" \
              "  --help - show this help message\n"                          \
              "If no arguments are provided, the agenda is enumerated.\n"    \
              , argv[0]


struct KeyValue {
	char *key;
	char *val;
};

struct KeyValue *key_value_read(const char *);

void buffer_append(char **, const char, size_t *);
void buffer_append_str(char **, const char *, size_t *);
