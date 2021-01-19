/* holds global config options, by default read from environment variables in
 * main.c
 */
typedef struct {
	const char *date_format;
	const char *time_format;
	/* week_start not actually used yet - for future repeating events */
	const char *week_start;
	const char *four_digit_year;
	const char *today_color;
	const char *pcolors[3];
} Config;

bool conf_enabled(const char *);

unsigned int match_int(const char, const char *, const char *);
int get_nth(int, const char, const char *);

int  make_four_digits(int);
bool is_alphanumeric(char);
bool is_ascii_int(char);
