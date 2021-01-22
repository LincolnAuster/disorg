/* opaque struct to contain data for a .ev */
typedef struct {
	char *title;
	char *cat;
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

Event *event_new_empty(const struct config *);
Event *event_now(const struct config *);
Event *event_free(Event *);

void   event_disp(const Event *, const struct config *);
void   event_ndisp(const Event *, const struct config *);
void   event_vdisp(const Event *, const struct config *);
void   event_sdisp(const Event *);

void   event_insert(Event *, struct KeyValue *, const struct config *);
void   event_insert_category(Event *, const char *);
void   event_insert_date(Event *, const char *, const struct config *);
void   event_insert_misc(Event *, char *, const struct config *);
void   event_insert_priority(Event *, char *, const struct config *);
void   event_fill_from_text(Event *, FILE *, const struct config *);

int event_compare_time(Event *, Event *);
int event_compare_name(Event *, Event *);

EventTree *eventtree_new(void);
EventTree *eventtree_insert(EventTree *, Event *, int (*)(Event *, Event *));
void      *eventtree_free(EventTree *);
EventTree *eventtree_new_from_event(Event *); 
void       eventtree_in_order(EventTree *, const struct config *,
		              void (*)(const Event *, const struct config *));
void       eventtree_if(EventTree *, const char *, const struct config *,
		              void (*)(const Event *, const struct config *));
