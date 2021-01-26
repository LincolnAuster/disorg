#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../src/conf.h"
#include "../src/util.h"

struct KeyValueTest {
	char *s;
	struct KeyValue kv;
};

struct BufferAppendTest {
	char *a; char b; size_t s;
	char *r;
};

struct BufferAppendStrTest {
	char *a; char *b; size_t s;
	char *r;
};

static const struct KeyValueTest key_value_read_tests[] = {
	{ "!TITLE this is a test",   { "TITLE", "this is a test"   } },
	{ "!lorem ipsum dolor\\sit", { "lorem", "ipsum dolor\\sit" } },
	{ "ABC ABC",                 { "MISC",  "ABC ABC"          } },
};

static const struct BufferAppendTest buffer_append_tests[] = {
	{ "abc",  'd',  4, "abcd" },
	{ "thi ", '\0', 4, "thi " }
};

int
main(void)
{
	int passed, total, count;
	passed = total = 0;

	printf("TESTING: util.h\n");
	printf("         key_value_read\n");

	count = sizeof(key_value_read_tests) /
	                           sizeof(key_value_read_tests[0]);

	for (int i = 0; i < count; i++) {
		struct KeyValueTest test = key_value_read_tests[i];
		struct KeyValue *result = key_value_read(test.s);

		if (KEY_VALUE_CMP((*result), test.kv) == 0) {
			printf("             PASSED `%s`\n", test.s);
			passed++;
		} else {
			printf("%s failed, gave %s:%s\n",
			       test.s, result->key, result->val);
		}
		total++;
		free(result);
	}

	printf("         buffer_append\n");

	count = sizeof(buffer_append_tests) / sizeof(buffer_append_tests[0]);
	for (int i = 0; i < count; i++) {
		struct BufferAppendTest test = buffer_append_tests[i];
		char *s = strdup(test.a);
		buffer_append(&s, test.b, &test.s);

		if (strcmp(s, test.r) == 0) {
			printf("            PASSED: `%s`\n", test.r);
			passed++;
		} else {
			printf("%s failed, gave %s", test.r, s);
		}

		total++;
		free(s);
	}

	printf("         buffer_append_str\n");

	float percent = (float) passed / total * 100;
	printf("%d/%d %.2f%%\n", passed, total, percent);

	return total - passed;
}
