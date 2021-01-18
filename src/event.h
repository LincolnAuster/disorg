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
void   event_disp(const Event *);
void   event_sdisp(const Event *);
void   event_ndisp(const Event *);
void   event_vdisp(const Event *);
void   event_insert(Event *, struct KeyValue *, const Config *);
void   event_insert_date(Event *, const char *, const Config *);
void   event_insert_misc(Event *, char *, const Config *);
void   event_fill_from_text(Event *, FILE *, const Config *);

int event_compare_time(Event *, Event *);
int event_compare_name(Event *, Event *);

EventTree *eventtree_new();
EventTree *eventtree_insert(EventTree *, Event *, int (*)(Event *, Event *));
void      *eventtree_free(EventTree *);
EventTree *eventtree_new_from_event(Event *); 
void       eventtree_in_order(EventTree *, void (*)(const Event *));
void       eventtree_if(EventTree *, char *, void (*)(const Event *));
