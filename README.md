# Mini Howto

The following section provides a small howto to download & install perf-studio,
generate a small test program, configure the first project, and start
perf-studio for first analysis.


Everything happends temporary (/tmp)
````
export D=/tmp
cd $D
````

Clone perf-studio master
````
git clone https://github.com/hgn/perf-studio
````

Install dependencies (Debian based distribution)
````
sudo apt-get install libwebkitgtk-3.0-dev libgtk-3-dev libdbus-1-dev
````

Compile and install perf-studio (to /tmp)
````
cd perf-studio
make prefix=$D install
````

Generate a stupid test program:
````
cat > test.c << EOT
#include <stdio.h>
int main(){
        int i = 1000;
        while (i--) {
                printf("Foo Bar\n");
        }
}
EOT
````

Compile program
````
gcc -g -o test test.c
````

Generate perf-studio project configuration file
````
$D/bin/perf-studio-ctrl project --create
````

A new project with an ID is created, the first project
ID is 0001 and is incremented each time a new project is created
(we assume 0001 for the example now)
Type '$D/bin/perf-studio-ctrl project --show 001' to get what happend

Now configure the project to analyze our small program:
````
$D/bin/perf-studio-ctrl project --set common.cmd "/tmp/test"
````

Finally start perf-studio
````
$D/bin/perf-studio
````


# Configuration

### Perf-studio Configuration

The user specific configuration is located under

`$HOME/.config/perf-studio/config`

Via "perf-studio-ctrtl config" the configuration can be created/modified/edited.
`perf-studio-ctrtl config --edit` to pop up a editor with the file loaded.

You can edit configuration values via the `--add` argument:

perf-studio-ctrtl config `--add common.username="John Doe"` common.perf-path=/usr/src/linux/tools/perf


### Project Configuration

Each perf-project is saved in $HOME/.cache/perf-studio/projects/. The unique ID
of the project is a increasing number. Removed project directories are recycled
if a new project is created, thus you cannot draw conclusions when a project
was created by looking at the ID.

All project related data is stored in ../ID/config - a INI style formated
configuration file. You can edit (but SHOULD not) by hand by calling:

`perf-studio-ctrtl project --edit <ID>`

E.g: `perf-studio-ctrtl project --edit 0001`


Example project configuration:

````
  [common]
    exec-path = /bin/ls
    exec-args = -al;/proc

  [times]
    create-time = 12230.101
    last-accessed = 12233.010
````



# Project Structure

````
$HOME/.cache/perf-studio/projects/
$HOME/.cache/perf-studio/projects/0001/
$HOME/.cache/perf-studio/projects/0001/config
$HOME/.cache/perf-studio/projects/0001/refs/2012-02-11-20:20.2949  ->  ../db/ee6a975d30943d63dc6f1685248ecbc3-000
$HOME/.cache/perf-studio/projects/0001/db/ee6a975d30943d63dc6f1685248ecbc3-000/
$HOME/.cache/perf-studio/projects/0001/db/ee6a975d30943d63dc6f1685248ecbc3-000/info
$HOME/.cache/perf-studio/projects/0001/db/ee6a975d30943d63dc6f1685248ecbc3-000/perf-00001.data
$HOME/.cache/perf-studio/projects/0001/db/ee6a975d30943d63dc6f1685248ecbc3-000/perf-00002.data
$HOME/.cache/perf-studio/projects/0001/db/ee6a975d30943d63dc6f1685248ecbc3-000/time-00001.data
````

The longer unique id the MD5 sum of the executable. This provides a clean
way to identify if a executable was modified (re-compiled) and measurements are
outdated. The last three digits are normally not used, but provide a way to
re-start measurement although the file is not modified. This can be useful if
new command-line arguments are given or the system was modified/optimized (like
changed sysctl)


The "info" file contains information about each data file. The most important:
what data is traced in the file, the format, etc, pp. The type key is required
and is a human readable form of the EVENT_TYPE_*, see include/perf-studio.h for
a list of supported events.

info:

````
  [perf-00001.data]
    type = PERF_SAMPLING
		events = cycles,10000,us;instructions,10000,us

  [perf-00002.data]
    type = PERF_SAMPLING
		events = cycles,10000,us;instructions,10000,us

  [time-00001.data]
    type = TIME
````



# Coding Style

Return Values:

They are generally three kinds of function classes:

1. Allocation functions return a pointer to a struct
   or NULL in the case of error
2. Status code with 0 (no error) or a negative value
   to indicate a error (e.g. -ENOMEM)
3. no success/error at all because the function can
   never fail.


# Function Name Nomenclature



````
/* long there objects. They normally return a
   pointer to a struct of type whatever_foo. To free
   the data structure you call whatever_foo_free(struct)
 */
whatever_foo_new();
````


````
/* long there objects. Not returning a object (maybe true or false)
   but register a object globaly somewhere.
 */
whatever_foo_setup();
````


````
/*
 * Short therm objects. In difference to the long term object
 * the create family of functions can return any data type:
 * int, struct, GSList, ...
 * Similarity is that both _new() and _create() will return a
 * new data type/value/object
 *
 * Some of them have associated free function 
 */
whatever_foo_create();
````

````
/* subsystem initialization without state. This class of
 * functions do not return a allocated object. Rather they
 * return success (0) or a negative failure code.  */
whatever_foo_init();
````


````
whatever_foo_update() functions are called in long term objects
````
