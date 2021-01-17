#!/bin/bash

BASE_DIRECTORY="$HOME/.disorg"
DATE_FORMAT="D-M-Y"
TIME_FORMAT="H:M"
FOUR_DIGIT_YEAR="TRUE"
DEBUG="FALSE"
source ~/.config/disorg/disorg 2> /dev/null

export FOUR_DIGIT_YEAR
export DATE_FORMAT
export TIME_FORMAT

if [[ "$1" == "COUNT" ]]; then
	find $BASE_DIRECTORY -type f -name "*.ev" | wc -l
fi

if [[ "$DEBUG" == "TRUE" ]]; then
	echo -e "$(find $BASE_DIRECTORY -type f)" |
		valgrind --leak-check=full $(dirname $0)/disorg-main "$@"
else
	echo -e "$(find $BASE_DIRECTORY -type f)" |
		$(dirname $0)/disorg-main "$@"
fi
