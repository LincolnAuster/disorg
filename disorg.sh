#!/bin/bash

BASE_DIRECTORY="$HOME/.disorg"
DATE_FORMAT="D-M-Y"
TIME_FORMAT="H:M"
FOUR_DIGIT_YEAR="TRUE"
DEBUG="FALSE"
source ~/.config/disorg/disorg

export FOUR_DIGIT_YEAR
export DATE_FORMAT
export TIME_FORMAT

if [[ "$DEBUG" == "TRUE" ]]; then
	echo -e "$(find $BASE_DIRECTORY -name '*.ev')" | valgrind --leak-check=full $(dirname $0)/disorg-main "$@"
else
	echo -e "$(find $BASE_DIRECTORY -name '*.ev')" | $(dirname $0)/disorg-main "$@"
fi
