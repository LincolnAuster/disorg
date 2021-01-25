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

static const struct KeyValueTest key_value_read_testcases[] = {
	"!TITLE this is a test", { "TITLE", "this is a test" }
};

int
main(void)
{
	int passed, total;
	passed = total = 0;

	printf("TESTING: util.h\n");
	printf("         key_value_read\n");

	int key_value_read_count = sizeof(key_value_read_testcases) /
	                           sizeof(key_value_read_testcases[0]);

	for (int i = 0; i < key_value_read_count; i++) {
		struct KeyValueTest test = key_value_read_testcases[i];
		struct KeyValue *result = key_value_read(test.s);

		if (KEY_VALUE_CMP((*result), test.kv) == 0) {
			printf("         PASSED `%s'\n");
			passed++;
		} else {
			printf("%s failed, gave %s:%s\n",
			       test.s, result->key, result->val);
		}
		total++;
		free(result);
	}

	float percent = passed / total * 100;
	printf("%d/%d --- %.2f%%\n", passed, total, percent);

	return total - passed;
}
