#define LEN(a) sizeof(a) / sizeof(a[0])

#define GREEN "\033[1;32m"
#define RESET "\033[0;m"

/* structs define input and expected output for a given function.
 * Formatting is as follows:
 *     struct *Test {
 *            type input_a; type input_b; ...;
 *            type output;
 *     }
 */

struct KeyValueReadTest {
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

struct ParentDirTest {
	char *a;
	char *p;
};

/* test cases are defined below, as constant arrays of the corresponding
 * structures. */

const struct KeyValueReadTest key_value_read_tests[] = {
	{ "!TITLE this is a test",   { "TITLE", "this is a test"   } },
	{ "!lorem ipsum dolor\\sit", { "lorem", "ipsum dolor\\sit" } },
	{ "ABC ABC",                 { "MISC",   "ABC ABC"         } },
};

const struct BufferAppendTest buffer_append_tests[] = {
	{ "abc",  'd',  4, "abcd" },
	{ "thi ", '\0', 4, "thi " }
};

const struct BufferAppendStrTest buffer_append_str_tests[] = {
	{ "abc",   "def",     4, "abcdef"   },
	{ " thi",  "s is",    5, " this is" },
	{ "\0abc", "garbage", 4, "garbage"  },
};

const struct ParentDirTest parent_dir_tests[] = {
	{ "/home/lincoln/Documents/disorg/tests/test.h", "tests" },
	{ "/mnt/nas/test.png",                           "nas"   }
};
