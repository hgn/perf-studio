CFLAGS=-g -O3 -pipe
ARCH ?= $(shell uname -m)

CC=gcc

BASIC_CFLAGS  = $(shell pkg-config --cflags gtk+-2.0)
BASIC_CFLAGS += $(shell pkg-config --cflags libxml-2.0)
BASIC_CFLAGS += $(shell pkg-config dbus-1 --cflags) -DDBUS_API_SUBJECT_TO_CHANGE

FLAGS = $(shell pkg-config --libs --cflags gtk+-2.0)

EXTLIBS += $(shell pkg-config --libs gtk+-2.0)
EXTLIBS += $(shell pkg-config --libs libxml-2.0)
EXTLIBS += $(shell pkg-config --libs dbus-1)

WARNINGS = -Wall \
					 -Wextra \
					 -Wwrite-strings \
					 -Wsign-compare \
					 -Wshadow \
					 -Wformat=2 \
					 -Wundef \
					 -Wstrict-prototypes \
					 -fno-strict-aliasing \
					 -fno-common \
					 -Wformat-security \
					 -Wformat-y2k \
					 -Winit-self \
					 -Wredundant-decls \
					 -Wstrict-aliasing=3 \
					 -Wswitch-default \
					 -Wswitch-enum \
					 -Wno-system-headers \
					 -Wundef \
					 -Wvolatile-register-var \
					 -Wcast-align \
					 -Wbad-function-cast \
					 -Wwrite-strings \
					 -Wold-style-definition \
					 -Wdeclaration-after-statement

# use 64 bit off_t etc. even on 32 bit systems
CFLAGS += -D_FILE_OFFSET_BITS=64 
CFLAGS += $(WARNINGS)

FILES  = perf-studio.c
FILES += shared.c
FILES += assistant.c
FILES += control-overview.c
FILES += control-pane.c
FILES += db.c
FILES += dbus.c
FILES += main-notebook.c
FILES += module.c
FILES += module-example.c
FILES += module-overview.c
FILES += run.c
FILES += status-widget.c
FILES += utils.c

H_FILES  = builtin-studio.h
H_FILES += assistant.h
H_FILES += control-pane.h
H_FILES += db.h
H_FILES += module.h
H_FILES += perf-studio.h
H_FILES += run.h
H_FILES += status-widget.h
H_FILES += utils.h
H_FILES += shared.h

FILES_H=
OBJECTS=$(subst .c,.o,$(FILES))


DESTDIR=/usr
BINDIR=/bin


TARGET=perf-studio

all: $(TARGET)

LIBS= -lssl -lz -lm

$(TARGET): $(OBJECTS)
	$(CC) $(LIBS) $(EXTLIBS) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c $(H_FILES)
	$(CC) $(CFLAGS) $(BASIC_CFLAGS) -c  $< -o $@

install: all
	install $(TARGET) $(DESTDIR)$(BINDIR)


clean:
	rm -f *.o $(TARGET) lstopo.png

