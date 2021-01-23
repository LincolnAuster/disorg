#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "conf.h"

/* abstract testing if config var is set or not */
bool
conf_enabled(const char *string)
{
	return (strcmp(string, "TRUE") == 0);
}

/* make a given year 4 digits long */
int
make_four_digits(int n)
{
	if (n > 999)
		return n;
	else
		return n + 2000;
}

/* given a format string (such as H:M:S) that corresponds to user data
 * (for instance 3:30:00), this returns the integer indicated by the 
 * marker character. Returns 0 if not matched. 
 */
unsigned int
match_int(char location_char, const char *string, const char *format_string)
{
	int location;
	char delim;
	delim     = first_nonalpha(format_string);

	location = char_location(location_char, delim, format_string);
        
	return get_nth(location, delim, string);
}

/* get location of a character in a format string, relative to delimiter */
unsigned int
char_location(char location_char, char delim, const char *format_string)
{
	int location, len;
	char c;

	location = 0;
	len = strlen(format_string);

	for (int i = 0; i < len; i++) {
		c = format_string[i];
		if (c == delim) {
			location++;
		} else if (c == location_char) {
			return location;
		}
	}

	return 0;
}

/* returns the first non-alphanumeric char */
char
first_nonalpha(const char *s)
{
	char c;
	size_t len = strlen(s);
	for (int i = 0; i < len; i++) {
		c = s[i];
		if (!is_alphanumeric(c))
			return c;
	}
	return '\0';
}

unsigned int
get_nth(int location, const char delim, const char *string)
{
	char *copy, *copy_a, *token;
	char delim_str[2];

	token = NULL;
	// delim_str = malloc(2 * sizeof(char));
	delim_str[0] = delim;
	delim_str[1] = '\0';

	copy_a = strdup(string);
	copy   = copy_a;

	for (int i = 0; i < location + 1; i++) {
		token = strsep(&copy, delim_str);
		if (token == NULL) {
			free(copy_a);
			return 0;
		}
	}


	int ans = atoi(token);
	free(copy_a);
	return ans;
}

/* returns true if c is either strictly a number 0-9 or a letter
 * a-Z.
 */
bool
is_alphanumeric(char c)
{
	return  ((is_ascii_int(c)     ||    
		 (64 < c && c < 91 )) || /* lowercase */
		((96 < c && c < 122)));  /* uppercase */
}

/* returns true if c is 0-9 */
bool
is_ascii_int(char c)
{
	return (47 < c && c < 58);
}
