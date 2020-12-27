// requires include of stdbool.h, string.h

/* holds global config options, by default read from environment variables in
 * main.c
 */
struct Config {
	const char *date_format;
	const char *time_format;
	const char *week_start;
	const char *four_digit_year; // TODO change to boolean
};

bool conf_enabled(const char *);

int make_four_digits(int);
bool is_alphanumeric(char);
bool is_ascii_int(char);
unsigned int match_int(int, const char *, const char *);
