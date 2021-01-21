#define CLI_OUTPUT_LEN 80
/* these numbers are all random guesses - i've tested nothing */
#define INITIAL_BUFFER_SIZE 12
#define HASH_SIZE           16

#define EVENT_EXT ".ev"
#define WIKI_EXT  ".wi"
#define WIKI_ARG "-W"

#define RESET_COLOR "\033[0m"
#define PRIORITY_STR_HIGH "HIGH"
#define PRIORITY_STR_MID  "MED"
#define PRIORITY_STR_LOW  "LOW"

struct KeyValue {
	char *key;
	char *val;
};

struct KeyValue *key_value_read(const char *);

void buffer_append(char **, const char, size_t *);
void buffer_append_str(char **, const char *, size_t *);
unsigned int hash_str(char *);

void die(const char *);

struct tm *tm_empty(void);
char *tm_tascii(const struct tm *);
char *tm_dascii(const struct tm *, const struct config *);
