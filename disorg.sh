#!/bin/bash

if [[ "$1" == "--help" ]]; then
	echo "     Usage: $0 [-W] [COUNT] [target] [--help]"
	echo "       -w : query wiki files as opposed to event files."
	echo "    COUNT : return the number of events currently in the base"
	echo "            directory."
	echo "   target : Return more detailed information about event or wiki"
	echo "            wiki page with title \`target', i.e.:"
	echo "                $0 [-W] \"Calculus Week 18 Lecture\""
	echo "   --help : Displays this help message."
	echo
	echo "If no arguments are provided, event files are enumerated."
	exit 0
fi

# COUNT can be specified to return the number of events, in total, located
# in the BASE_DIRECTORY. This does _not_ check if the events are valid,
# just the existance of the file.
if [[ "$1" == "COUNT" ]]; then
	echo "COUNT: $(find $BASE_DIRECTORY -type f -name "*.ev" | wc -l)"
	exit 0
fi

# main.c loads config from environment variables, so defaults are set and the
# config is loaded in (the redirection to /dev/null prevents an error message
# if the config file doesn't exist. DEBUG is not loaded by main.c; if enabled
# the script runs valgrind to check for memory leaks.

# disorg.sh options
BASE_DIRECTORY="$HOME/.disorg"
DEBUG="FALSE"
PATTERN="*.ev"

# The W option controls a) the files that are passed to the binary, and b) the
# order in which the files are sorted on output - wiki files have no associated
# time, and as such cannot be sorted by that attribute. Instead, they are sorted
# alphabetically.
if [[ "$@" == *"-W"* ]]; then
	PATTERN="*.wi"
fi

# disorg-main config file options
DATE_FORMAT="D-M-Y"
TIME_FORMAT="H:M"
FOUR_DIGIT_YEAR="TRUE"

TODAY_COLOR='91'
PHIGH_COLOR='31'
PMED_COLOR='35'
PLOW_COLOR='0'

CATEGORY_COLORS="13 14 15 61 73 214"

source ~/.config/disorg/disorg 2> /dev/null
export FOUR_DIGIT_YEAR
export DATE_FORMAT
export TIME_FORMAT
export TODAY_COLOR
export PHIGH_COLOR
export PMED_COLOR
export PLOW_COLOR

# disorg-main CLI args
TARGET=""
CATEGORY=""
WIKI="FALSE"

# assume that target is the last non option argument (doesn't begin with -)
for arg in "$@"; do
	if [[ "$arg" != "-"* ]]; then
		TARGET="$arg"
		break
	fi
done

# check from pattern
if [[ "$PATTERN" == "*.wi" ]]; then
	WIKI="TRUE"
fi

# If the -C option is specified, limit the BASE_DIRECTORY to that subdirectory.
for ((i = 1 ; i < $# ; i++ )); do
	if [[ "${!i}" == "-C" ]]; then
		i=$(($i + 1))
		CATEGORY="${!i}"
	fi
done

export CATEGORY
export TARGET
export WIKI

FILES=$(find $BASE_DIRECTORY -type f -name $PATTERN)
# Call the main binary: write all files matching args to stdin.
if [[ "$DEBUG" == "TRUE" ]]; then
	time 2> valgrind.log valgrind --leak-check=full \
	   $(dirname $0)/disorg-main "$@" <<< $FILES
else
	$(dirname $0)/disorg-main "$@" <<< "$FILES"
fi
