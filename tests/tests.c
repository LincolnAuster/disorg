#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../src/conf.h"
#include "../src/util.h"

#include "tests.h"

static int passed, total;

/* access static vars to test respective functions */
static void key_value_read_test(void);

static void buffer_append_test(void);
static void buffer_append_str_test(void);
/* buftocol doesn't really need testing, its behaviour is supposed to be random
 * anyway */
static void parent_dir_test(void);

static void
key_value_read_test(void)
{
	int count = LEN(key_value_read_tests);
	for (int i = 0; i < count; i++) {
		struct KeyValueReadTest test = key_value_read_tests[i];
		struct KeyValue *result = key_value_read(test.s);

		if (KEY_VALUE_CMP((*result), test.kv) == 0) {
			printf("PASSED `%s`\n", test.s);
			passed++;
		} else {
			printf("%s FAILED, gave %s:%s\n",
			       test.s, result->key, result->val);
		}
		total++;
		free(result);
	}
}

static void
buffer_append_test(void)
{
	int count = LEN(buffer_append_tests);
	for (int i = 0; i < count; i++) {
		struct BufferAppendTest test = buffer_append_tests[i];
		char *s = strdup(test.a);
		buffer_append(&s, test.b, &test.s);

		if (strcmp(s, test.r) == 0) {
			printf("PASSED: `%s`\n", test.r);
			passed++;
		} else {
			printf("%s FAILED, gave %s\n", test.r, s);
		}

		total++;
		free(s);
	}
}

static void
buffer_append_str_test(void)
{
	int count = LEN(buffer_append_str_tests);
	for (int i = 0; i < count; i++) {
		struct BufferAppendStrTest test = buffer_append_str_tests[i];
		char *a = strdup(test.a);
		buffer_append_str(&a, test.b, &test.s);

		if (strcmp(a, test.r) == 0) {
			printf("PASSED: `%s`\n", test.r);
			passed++;
		} else {
			printf("%s FAILED, gave `%s`\n", test.r,  a);
		}

		total++;
		free(a);
	}
}

int
main(void)
{
	passed = total = 0;

	printf(GREEN "TESTING: util.h\n" RESET);
	printf(GREEN "         key_value_read\n" RESET );
	key_value_read_test();

	printf(GREEN "         buffer_append\n" RESET);
	buffer_append_test();

	printf(GREEN "         buffer_append_str\n" RESET);
	buffer_append_str_test();

	float percent = (float) passed / total * 100;
	printf("%d/%d %.2f%%\n", passed, total, percent);

	return total - passed;
}
