# Linux kbuild and trace-cmd parts included

# Makefiles suck: This macro sets a default value of $(2) for the
# variable named by $(1), unless the variable has been set by
# environment or command line. This is necessary for CC and AR
# because make sets default values, so the simpler ?= approach
# won't work as expected.
define allow-override
  $(if $(or $(findstring environment,$(origin $(1))),\
            $(findstring command line,$(origin $(1)))),,\
    $(eval $(1) = $(2)))
endef

# Allow setting CC and AR, or setting CROSS_COMPILE as a prefix.
$(call allow-override,CC,$(CROSS_COMPILE)gcc)
$(call allow-override,AR,$(CROSS_COMPILE)ar)

# Use DESTDIR for installing into a different root directory.
# This is useful for building a package. The program will be
# installed in this directory as if it was the root directory.
# Then the build tool can move it later.
DESTDIR ?=
DESTDIR_SQ = '$(subst ','\'',$(DESTDIR))'

prefix ?= /usr
bindir_relative = bin
bindir = $(prefix)/$(bindir_relative)
man_dir = $(prefix)/share/man
man_dir_SQ = '$(subst ','\'',$(man_dir))'
img_install = $(prefix)/share/kernelshark/html/images
img_install_SQ = '$(subst ','\'',$(img_install))'

export img_install img_install_SQ
export DESTDIR DESTDIR_SQ

ifeq ($(prefix),$(HOME))
module_dir = $(HOME)/.perf-studio/modules
else
module_dir = $(prefix)/lib/perf-studio/modules
MODULE_DIR = -DMODULE_DIR="\"$(module_dir)\""
MODULE_DIR_SQ = '$(subst ','\'',$(MODULE_DIR))'
endif

ifeq ("$(origin V)", "command line")
  VERBOSE = $(V)
endif
ifndef VERBOSE
  VERBOSE = 0
endif


# Shell quotes
bindir_SQ = $(subst ','\'',$(bindir))
bindir_relative_SQ = $(subst ','\'',$(bindir_relative))
module_dir_SQ = $(subst ','\'',$(module_dir))

CFLAGS ?= -g -O0 -pipe
INSTALL = install

ARCH ?= $(shell uname -m)
MAKEFLAGS += --no-print-directory

CC=gcc

BASIC_CFLAGS  = $(shell pkg-config --cflags gtk+-3.0)
BASIC_CFLAGS += $(shell pkg-config --cflags libxml-2.0)
BASIC_CFLAGS += $(shell pkg-config --cflags dbus-1)

EXTLIBS  = $(shell pkg-config --libs gtk+-3.0)
EXTLIBS += $(shell pkg-config --libs libxml-2.0)
EXTLIBS += $(shell pkg-config --libs dbus-1)
EXTLIBS += -lpthread
EXTLIBS += -lm
EXTLIBS += -ldl

WARNINGS = -Wall \
					 -Wextra \
					 -Wwrite-strings \
					 -Wsign-compare \
					 -Wshadow \
					 -Wformat=2 \
					 -Wundef \
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
override CFLAGS += -D_FILE_OFFSET_BITS=64
override CFLAGS += $(WARNINGS)
override CFLAGS += -I../include
override CFLAGS += $(BASIC_CFLAGS) $(MODULE_DIR)

GOBJ		= $@

ifeq ($(VERBOSE),1)
  Q =
  print_compile =
  print_app_build =
  print_fpic_compile =
  print_shared_lib_compile =
  print_module_obj_compile =
  print_module_build =
  print_install =
else
  Q = @
  print_compile =		echo '  $(GUI)COMPILE            '$(GOBJ);
  print_app_build =		echo '  $(GUI)BUILD              '$(GOBJ);
  print_fpic_compile =		echo '  $(GUI)COMPILE FPIC       '$(GOBJ);
  print_shared_lib_compile =	echo '  $(GUI)COMPILE SHARED LIB '$(GOBJ);
  print_module_obj_compile =	echo '  $(GUI)COMPILE MODULE     '$(GOBJ);
  print_module_build =		echo '  $(GUI)BUILD MODULE       '$(GOBJ);
  print_static_lib_build =	echo '  $(GUI)BUILD STATIC LIB   '$(GOBJ);
  print_install =		echo '  $(GUI)INSTALL     '$(GSPACE)$1'	to	$(DESTDIR_SQ)$2';
endif

do_fpic_compile =					\
	($(print_fpic_compile)				\
	$(CC) -c $(CFLAGS) $(EXT) -fPIC $< -o $@)

do_app_build =						\
	($(print_app_build)				\
	$(CC) $^ -rdynamic -o $@ $(LDFLAGS) $(CONFIG_LIBS) $(EXTLIBS) $(LIBS))

do_compile_shared_library =			\
	($(print_shared_lib_compile)		\
	$(CC) --shared $^ -o $@)

do_compile_module_obj =				\
	($(print_module_obj_compile)		\
	$(CC) -c $(CFLAGS) $(BASIC_CFLAGS) -fPIC -o $@ $<)

do_module_build =				\
	($(print_module_build)			\
	$(CC) $(CFLAGS) $(EXTLIBS) $(LDFLAGS) -shared -nostartfiles -o $@ $<)

do_build_static_lib =				\
	($(print_static_lib_build)		\
	$(RM) $@;  $(AR) rcs $@ $^)


FILES  = perf-studio.c
FILES += shared.c
FILES += modules.c
FILES += module-utils.c
FILES += event.c
FILES += conf-file.c
FILES += cpu-features.c

H_FILES  = perf-studio.h
H_FILES += shared.h
H_FILES += modules.h
H_FILES += modules-utils.h
H_FILES += ../include/perf-studio.h

COBJECTS=$(subst .c,.o,$(FILES))


DESTDIR=/usr
BINDIR=/bin

TARGET=perf-studio

all: $(COBJECTS) $(TARGET)

$(TARGET): $(COBJECTS)
	$(Q)$(do_app_build)

define check_gui
	$(print_compile) \
	$(CC) -c $(CFLAGS) $(EXT) $< -o $@;
endef

$(obj)/%.o: %.c
	$(Q)$(call check_gui)

%.o: %.c
	$(Q)$(call check_gui)


install: all 
	install $(TARGET) $(DESTDIR)$(BINDIR)

define do_install
	$(print_install)				\
	if [ ! -d '$(DESTDIR_SQ)$2' ]; then		\
		$(INSTALL) -d -m 755 '$(DESTDIR_SQ)$2';	\
	fi;						\
	$(INSTALL) $1 '$(DESTDIR_SQ)$2'
endef

clean:
	$(RM) *.o *.so $(TARGET) core*
	$(RM) cscope*

memcheck: $(TARGET)
	valgrind --tool=memcheck ./$(TARGET)

cscope:
	$(RM) cscope*
	find .. -name '*.[ch]' | cscope -b -q
