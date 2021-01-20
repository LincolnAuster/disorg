typedef enum {
	low,
	med,
	high,
} Priority;
/* opaque struct to contain data for a .ev */
typedef struct {
	char *title;
	char *description;
	char *misc;
	size_t misc_cap;

	struct tm *time;

	int p;
	bool short_disp;
} Event;

typedef struct EventTree {
	Event            *event;
	struct EventTree *left;
	struct EventTree *right;
} EventTree;

Event *event_new_empty(const Config *);
Event *event_now(const Config *);
Event *event_free(Event *);

void   event_disp(const Event *, const Config *);
void   event_ndisp(const Event *, const Config *);
void   event_vdisp(const Event *, const Config *);
void   event_sdisp(const Event *);

void   event_insert(Event *, struct KeyValue *, const Config *);
void   event_insert_date(Event *, const char *, const Config *);
void   event_insert_misc(Event *, char *, const Config *);
void   event_insert_priority(Event *, char *, const Config *);
void   event_fill_from_text(Event *, FILE *, const Config *);

int event_compare_time(Event *, Event *);
int event_compare_name(Event *, Event *);

EventTree *eventtree_new();
EventTree *eventtree_insert(EventTree *, Event *, int (*)(Event *, Event *));
void      *eventtree_free(EventTree *);
EventTree *eventtree_new_from_event(Event *); 
void       eventtree_in_order(EventTree *, const Config *,
		              void (*)(const Event *, const Config *));
void       eventtree_if(EventTree *, char *, const Config *,
		              void (*)(const Event *, const Config *));
