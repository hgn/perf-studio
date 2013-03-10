# Linux kbuild and trace-cmd parts included

PS_VERSION = 0
PS_PATCHLEVEL = 1
PS_EXTRAVERSION = 0

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
DESTDIR ?= /usr
BINDIR=/bin
DESTDIR_SQ = '$(subst ','\'',$(DESTDIR))'

prefix ?= /usr
bindir_relative = bin
bindir = $(prefix)/$(bindir_relative)
man_dir = $(prefix)/share/man
man_dir_SQ = '$(subst ','\'',$(man_dir))'
img_install = $(prefix)/share/kernelshark/html/images
img_install_SQ = '$(subst ','\'',$(img_install))'
data_dir = $(prefix)/share/perf-studio/

export data_dir
export img_install img_install_SQ
export DESTDIR DESTDIR_SQ

ifeq ($(prefix),$(HOME))
module_dir = $(HOME)/.perf-studio/modules
else
module_dir = $(prefix)/lib/perf-studio/modules
MODULE_DIR = -DMODULE_DIR="\"$(module_dir)\""
MODULE_DIR_SQ = '$(subst ','\'',$(MODULE_DIR))'
endif

DATA_DIR = -DDATA_DIR="\"$(data_dir)\""

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

CFLAGS ?= -g -pipe
INSTALL = install

ARCH ?= $(shell uname -m)
MAKEFLAGS += --no-print-directory


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
  print_rm =
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
  print_rm =		echo '  $(GUI)CLEAN      ';
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


define check_gui
	$(print_compile) \
	$(CC) -c $(CFLAGS) $(EXT) $< -o $@;
endef


# foo
VERSION		= $(PS_VERSION)
PATCHLEVEL	= $(PS_PATCHLEVEL)
EXTRAVERSION	= $(PS_EXTRAVERSION)

PS_CMD_VERSION = $(PS_VERSION).$(PS_PATCHLEVEL).$(PS_EXTRAVERSION)

define make_version.h
	(echo '/* This file is automatically generated. Do not modify. */';		\
	echo \#define VERSION_CODE $(shell						\
	expr $(VERSION) \* 256 + $(PATCHLEVEL));					\
	echo '#define EXTRAVERSION ' $(EXTRAVERSION);					\
	echo '#define VERSION_STRING "'$(VERSION).$(PATCHLEVEL).$(EXTRAVERSION)'"';	\
	echo '#define FILE_VERSION '$(FILE_VERSION);					\
	) > $1
endef

define update_version.h
	($(call make_version.h, $@.tmp);		\
	if [ -r $@ ] && cmp -s $@ $@.tmp; then		\
		rm -f $@.tmp;				\
	else						\
		echo '  UPDATE             $@';	\
		mv -f $@.tmp $@;			\
	fi);
endef


define make_authors.h
	(echo '/* This file is automatically generated. Do not modify. */';		\
	echo "static char authors[][64] {"; \
	echo $(shell git log --format='{\"%aN\"},' | sort -u);						\
	echo "};"; \
	) > $1
endef

define update_authors.h
	($(call make_authors.h, $@.tmp);		\
	if [ -r $@ ] && cmp -s $@ $@.tmp; then		\
		rm -f $@.tmp;				\
	else						\
		echo '  UPDATE             $@';	\
		mv -f $@.tmp $@;			\
	fi);
endef

define check_gui_deps
		$(CC) -M $(CFLAGS) $< > $@
endef


define do_install
	$(print_install)				\
	if [ ! -d '$(DESTDIR_SQ)$2' ]; then		\
		$(INSTALL) -d -m 755 '$(DESTDIR_SQ)$2';	\
	fi;						\
	$(INSTALL) $1 '$(DESTDIR_SQ)$2'
endef

