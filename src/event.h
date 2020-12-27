#define CLI_OUTPUT_LEN 80

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

Event *event_new_empty(const struct Config *);
void   event_display(Event *);
void   event_insert(Event *, struct KeyValue *, const struct Config *);

int event_compare(Event *, Event *);