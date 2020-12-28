#define CLI_OUTPUT_LEN 80
#define INITIAL_BUFFER_SIZE 4

/* opaque struct to contain data for a .ev */
typedef struct {
	char *title;
	char *description;
	unsigned int hour;
	unsigned int minute;
	unsigned int day;
	unsigned int month;
	unsigned int year;
} Event;

/* Stores a key value pair that can be returned from a parsing function. */
struct KeyValue {
	char *key;
	char *val;
};

struct KeyValue *key_value_read(char *);
void buffer_append(char **, char, size_t *);

Event *event_new_empty(const struct Config *);
void   event_display(Event *);
void   event_insert(Event *, struct KeyValue *, const struct Config *);
void   event_fill_from_text(Event *, FILE *, const struct Config *);

int event_compare(Event *, Event *);