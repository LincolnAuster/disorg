#!/bin/bash

# main.c loads config from environment variables, so defaults are set and the
# config is loaded in (the redirection to /dev/null prevents an error message
# if the config file doesn't exist. DEBUG is not loaded by main.c; if enabled
# the script runs valgrind to check for memory leaks.

BASE_DIRECTORY="$HOME/.disorg"
DATE_FORMAT="D-M-Y"
TIME_FORMAT="H:M"
FOUR_DIGIT_YEAR="TRUE"
TODAY_COLOR='91'
PHIGH_COLOR='31'
PMED_COLOR='35'
PLOW_COLOR='30'
DEBUG="FALSE"
source ~/.config/disorg/disorg 2> /dev/null
export FOUR_DIGIT_YEAR
export DATE_FORMAT
export TIME_FORMAT
export TODAY_COLOR
export PHIGH_COLOR
export PMED_COLOR
export PLOW_COLOR

# COUNT can be specified to return the number of events, in total, located
# in the BASE_DIRECTORY. This does _not_ check if the events are valid,
# just the existance of the file.
if [[ "$1" == "COUNT" ]]; then
	find $BASE_DIRECTORY -type f -name "*.ev" | wc -l
	exit 0
fi

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
fi

# The W option controls a) the files that are passed to the binary, and b) the
# order in which the files are sorted on output - wiki files have no associated
# time, and as such cannot be sorted by that attribute. Instead, they are sorted
# alphabetically.
PATTERN="*.ev"
if [[ "$*" == *"-W"* ]]; then
	PATTERN="*.wi"
fi

FILES=$(find $BASE_DIRECTORY -type f -name $PATTERN)
# Call the main binary: write all files matching args to stdin.
if [[ "$DEBUG" == "TRUE" ]]; then
	valgrind --leak-check=full $(dirname $0)/disorg-main "$@" <<< $FILES
else
	$(dirname $0)/disorg-main "$@" <<< $FILES
fi
