#define INITIAL_BUFFER_SIZE 12

#define EVENT_EXT ".ev"
#define WIKI_EXT  ".wi"
#define WIKI_ARG "-W"

#define RESET_COLOR "\033[0m"
#define PRIORITY_STR_HIGH "HIGH"
#define PRIORITY_STR_MID  "MED"
#define PRIORITY_STR_LOW  "LOW"

#define KEY_VALUE_CMP(a, b) (strcmp(a.key, b.key) + strcmp(a.val, b.val))

struct KeyValue {
	char *key;
	char *val;
};

struct KeyValue *key_value_read(const char *);

void buffer_append(char **, const char, size_t *);
void buffer_append_str(char **, const char *, size_t *);
unsigned int buftocol(const char *);

char *parent_dir(const char *);

void die(const char *);

struct tm *tm_empty(void);
char *tm_tascii(const struct tm *);
char *tm_dascii(const struct tm *, const struct config *);

void print_long(const char *, size_t);
