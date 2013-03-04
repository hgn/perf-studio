include ../project.mk

FILES  = perf-studio.c
FILES += shared.c
FILES += modules.c
FILES += module-utils.c
FILES += event.c
FILES += conf-file.c
FILES += cpu-features.c
FILES += project.c
FILES += random.c

FILES += gui-main.c
FILES += gui-amc.c
FILES += gui-amn.c
FILES += gui-apo.c
FILES += gui-atitle.c
FILES += gui-statusbar.c

FILES += disect-core.c
FILES += disect-perf.c

FILES += system-info.c


BASIC_CFLAGS  = $(shell pkg-config --cflags gtk+-3.0)
BASIC_CFLAGS += $(shell pkg-config --cflags libxml-2.0)
BASIC_CFLAGS += $(shell pkg-config --cflags dbus-1)

EXTLIBS  = $(shell pkg-config --libs gtk+-3.0)
EXTLIBS += $(shell pkg-config --libs libxml-2.0)
EXTLIBS += $(shell pkg-config --libs dbus-1)
EXTLIBS += -lpthread
EXTLIBS += -lm
EXTLIBS += -ldl
EXTLIBS += ../gui-toolkit/gui-toolkit.a

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
override CFLAGS += -D_GNU_SOURCE
override CFLAGS += $(WARNINGS)
override CFLAGS += -I../include
override CFLAGS += -I../gui-toolkit
override CFLAGS += $(BASIC_CFLAGS)
override CFLAGS += $(MODULE_DIR)
override CFLAGS += $(DATA_DIR)
override CFLAGS += -std=gnu99

COBJECTS=$(subst .c,.o,$(FILES))

TARGET=perf-studio

all: $(COBJECTS) $(TARGET)

$(TARGET): $(COBJECTS)
	$(Q)$(do_app_build)

$(obj)/%.o: %.c
	$(Q)$(call check_gui)

%.o: %.c
	$(Q)$(call check_gui)

version.h: force
	$(Q)$(N)$(call update_version.h)

## make deps

all_objs := $(sort $(COBJECTS))
all_deps := $(all_objs:%.o=.%.d)
non_gui_deps = $(all_deps)


$(non_gui_deps): version.h

$(all_deps): .%.d: %.c
	$(Q)$(call check_gui_deps)

$(all_objs) : %.o : .%.d

dep_includes := $(wildcard $(non_gui_deps))

ifneq ($(dep_includes),)
 include $(dep_includes)
endif


install: all 
	install $(TARGET) $(DESTDIR)$(BINDIR)

do_clean =						\
	($(print_rm)				\
	$(RM) *.o *.so $(TARGET) core* \
	$(RM) cscope* \
	$(RM) version.h \
	$(RM) .*.d)

clean:
	$(Q)$(do_clean)

PHONY += force
force:

# Declare the contents of the .PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.
.PHONY: $(PHONY)
