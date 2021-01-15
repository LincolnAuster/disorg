#define CLI_OUTPUT_LEN 80
#define INITIAL_BUFFER_SIZE 12

struct KeyValue {
	char *key;
	char *val;
};

struct KeyValue *key_value_read(const char *);
