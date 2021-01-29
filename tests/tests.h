#define LEN(a) sizeof(a) / sizeof(a[0])

#define PASSED "PASSED `%s`\n"
#define FAILED "FAILED (%s gave %s)\n"

#define GREEN "\033[1;32m"
#define RESET "\033[0;m"

/* structs define input and expected output for a given function.
 * Formatting is as follows:
 *     struct *Test {
 *            type input_a; type input_b; ...;
 *            type output;                     i.e., char *r(esult)
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

struct TmTasciiTest {
	struct tm tm;
	char *r;
};

struct TmDasciiTest {
	struct tm tm; struct config c;
	char *r;
};

/* config structs for testing */
const struct config CONFIG_TESTS[] = {
	{ "D/M/Y", "H:M", "TRUE",
	"91", { "31", "35", "0" }, 80,
	0, 0, NULL, NULL,
	false },

	{ "D-M-Y", "H:M", "TRUE",
	"91", { "31", "35", "0" }, 80,
	0, 0, NULL, NULL,
	false },
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

const struct TmTasciiTest tm_tascii_tests[] = {
	{ { 0, 30, 12, 1, 0, 100, false }, "12:30" },
	{ { 1, 35, 0,  1, 0, 100, false }, "00:35" }
};

const struct TmDasciiTest tm_dascii_tests[] = {
	{ { 0, 30, 12, 1, 0, 100, false },  CONFIG_TESTS[0], "01/01/2000" },
	{ { 1, 35, 0,  29, 0, 121, false }, CONFIG_TESTS[0], "29/01/2021" },
	{ { 0, 30, 12, 1, 0, 100, false },  CONFIG_TESTS[1], "01-01-2000" },
	{ { 1, 35, 0,  29, 0, 121, false }, CONFIG_TESTS[1], "29-01-2021" }
};
