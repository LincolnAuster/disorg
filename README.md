<h1 align="center">disorg</h1>
<p align="center">disorganized program inpsired by emacs' org-mode<br/>a tool to maintain a schedule and agenda over a directory of plain-text files</p>

## Usage
`disorg` aggregates an agenda based off of events (plain text .ev files) located in a base directory (by default `~/.disorg`). When `disorg.sh` (not `disorg-main`) is run with no arguments, it displays an agenda, an ordered list containing overviews of all events in the base directory. The current date is marked relative to the events, i.e.:
```
--------------------------------------------------------------------------------
TODAY
--------------------------------------------------------------------------------
OCS25 Readings                                                             TITLE
Undirected graphs                                                    DESCRIPTION
00:00
18/01/2021
--------------------------------------------------------------------------------
German Lesson Backup Platform                                              TITLE
(null)                                                               DESCRIPTION
23:59
18/01/2021
--------------------------------------------------------------------------------
German Readings                                                            TITLE
Assignments 3 and 4                                                  DESCRIPTION
00:00
19/01/2021
--------------------------------------------------------------------------------
Schedule TAA Presentation                                                  TITLE
(null)                                                               DESCRIPTION
00:00
20/01/2021
--------------------------------------------------------------------------------
TAA Readings                                                               TITLE
(null)                                                               DESCRIPTION
00:00
01/20/2021
```
The `-W` flag can be passed to list wiki files (.wi plain text files) similarly in alphabetical order.

For a detailed view of one item (wiki or event), the name can be passed to the script (as a single argument: `"OCS25 Readings"` over `OCS25 Readings`).

Adding, moving, deleting, or otherwise modifying events is not done through this program, it should be done through a text editor.

### Events
.ev files have the following form:
```
!TITLE title
!DESCRIPTION a short description
!PRIORITY HIGH/MED/LOW

!DATE 1/1/2020
!TIME 00:00

any extra associated information
```

### Wiki
.wi files are identical to event files, but only specify a title.

## Building/Installing
`make && sudo make install`

## Configuration
Configuration is done through a shell script, located at `~/.config/disorg/disorg`. The wrapper script around the binary sets default values for the configuration options, source the config file, and then export the values, to be loaded by `getenv()` in the binary.

Current config options are:
```bash
BASE_DIRECTORY="$HOME/.disorg" # where disorg looks for event/wiki files
DATE_FORMAT="D-M-Y"
TIME_FORMAT="H:M"
FOUR_DIGIT_YEAR="TRUE"         # 2021 vs 21
TODAY_COLOR='91'               # the ANSI escape code color used to mark the current time/today
PHIGH_COLOR='31'               # The ANSI escape code color used to mark high-priority events
PMED_COLOR='35'                # The ANSI escape code color used to mark medium-priority events
PLOW_COLOR='0'                 # The ANSI escape color code used ot mark low-priority events.
                                 By default this is just the reset code to remove any color.
DEBUG="FALSE"                  # run with valgrind to check for memory leaks
```

By default: dates are separated by dashes and in day-month-year format. Times are expected to be in 24-hour form.

## TODOs
* ✨ make software good ✨
* tags for wikis and events
