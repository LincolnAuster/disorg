#include <stdlib.h>
#include <stdio.h>
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
	bool increment = true;
	int location = 0;
	char delim   = '\0';
	int len = strlen(format_string);
	for (int i = 0; i < len; i++) {
		if (!is_alphanumeric(format_string[i])) {
			delim = format_string[i];
			if (increment) location++;
		}

		if (location_char == format_string[i])
			increment = false;
	}
        
	if (delim == '\0')
		return 0;

	char *modifiable_copy = malloc(++len * sizeof(char));
	strcpy(modifiable_copy, string);

	char *result = { '\0' };
	const char delim_str[2] = {delim, '\0'};
	for (int i = 0; i < location + 1; i++) {
		result = strsep(&modifiable_copy, delim_str);
		if (result == NULL) return 0;
	}

	return atoi(result);
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