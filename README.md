<h1 align="center">disorg</h1>
<p align="center">disorganized program inpsired by emacs' org-mode<br/>a tool to maintain a schedule and agenda over a directory of plain-text files</p>

## Usage
When called over a directory (by default ~/.disorg, as specified in config), the binary will aggregate events and wiki pages. Events, marked by a .ev file extension, define a single time associated with a set of metadata (title, description, etc) as well as any free form text. Wiki files provide a searchable database of information, indexed by name.

When no arguments are provided, the agenda (ordered list of events in the given directory) is written to stdout. If there are any other arguments, the first is interpreted as a name, and the corresponding event is displayed in its entirety. This is the overview displayed in the agenda followed by any unlabeled text in the .ev file.

To add events or wiki pages, use a text editor. This program is only able to read events and exists purely to aggregate.

### Events
.ev files have the following form:
```
!TITLE title
!DESCRIPTION a short description

!DATE 1/1/2020
!TIME 00:00

any extra associated information
```

### Wiki
.wi files are identical to event files, but only specify a title.

## Configuration
Configuration is done through a shell script, located at `~/.config/disorg/disorg`. The wrapper script around the binary sets default values for the configuration options, source the config file, and then export the values, to be loaded by `getenv()` in the binary.

Current config options are:
```bash
DEBUG="FALSE"
BASE_DIRECTORY="$HOME/.disorg"
DATE_FORMAT="D-M-Y"
TIME_FORMAT="H:M"
```

By default: dates are separated by dashes and in day-month-year format. Times are expected to be in 24-hour form.

## Structure
A single binary parses event or wiki files as specified by the command line arguments given by the `disorg.sh` script.

## TODOs
✨ make software good ✨
