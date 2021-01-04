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
	int location, len;
	char delim, c;
	bool increment;

	location  = 0;
	len       = strlen(format_string);
	delim     = '\0';
	increment = true;

	for (int i = 0; i < len; i++) {
		c = format_string[i];
		if (!is_alphanumeric(format_string[i])) {
			delim = c;
			if (increment) location++;
		}

		if (location_char == c)
			increment = false;
	}
        
	if (delim == '\0')
		return 0;

	return get_nth(location, delim, string);
}

int
get_nth(int location, const char delim, const char *string)
{
	char *modifiable_copy = strdup(string);
	char *result = NULL;
	const char delim_str[2] = {delim, '\0'};

	for (int i = 0; i < location + 1; i++) {
		result = strsep(&modifiable_copy, delim_str);
		if (result == NULL) {
			free(modifiable_copy);
			return 0;
		}
	}

	int ans = atoi(result);
	free(modifiable_copy);
	free(result);
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
