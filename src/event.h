#define CLI_OUTPUT_LEN 80
#define INITIAL_BUFFER_SIZE 8 /* this is just a random guess, i've tested nothing */

/* opaque struct to contain data for a .ev */
typedef struct {
	char *title;
	char *description;
	char *misc;
	unsigned int hour;
	unsigned int minute;
	unsigned int day;
	unsigned int month;
	unsigned int year;
} Event;

typedef struct EventTree {
	Event            *event;
	struct EventTree *left;
	struct EventTree *right;
} EventTree;

/* Stores a key value pair that can be returned from a parsing function. */
struct KeyValue {
	char *key;
	char *val;
};

struct KeyValue *key_value_read(const char *);

void buffer_append(char **, const char, size_t *);
void buffer_append_str(char **, const char *, size_t *);

Event *event_new_empty(const Config *);
Event *event_free(Event *);
void   event_disp(Event *);
void   event_vdisp(Event *);
void   event_insert(Event *, struct KeyValue *, const Config *);
void   event_insert_date(Event *, const char *, const Config *);
void   event_insert_misc(Event *, char *, const Config *);
void   event_fill_from_text(Event *, FILE *, const Config *);

int event_compare(Event *, Event *);

EventTree *eventtree_new();
EventTree *eventtree_insert(EventTree *, Event *);
void      *eventtree_free(EventTree *);
EventTree *eventtree_new_from_event(Event *); 
void       eventtree_in_order(EventTree *, void (*)(Event *));
void       eventtree_if(EventTree *, char *, void (*)(Event *));
