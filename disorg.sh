#!/bin/bash

BASE_DIRECTORY="$HOME/.disorg"
DATE_FORMAT="D-M-Y"
TIME_FORMAT="H:M"
WEEK_START="MON"
FOUR_DIGIT_YEAR="TRUE"
source ~/.config/disorg/disorg

export BASE_DIRECTORY

export FOUR_DIGIT_YEAR
export DATE_FORMAT
export TIME_FORMAT
export WEEK_START

echo -e "$(find $BASE_DIRECTORY -name '*.ev')" | $(dirname $0)/disorg-main "$@"