include ../project.mk

FILES  = canvas.c
FILES += configuration.c
FILES += font.c
FILES += pie-chart.c
FILES += icons.c

BASIC_CFLAGS  = $(shell pkg-config --cflags gtk+-3.0)

EXTLIBS  = $(shell pkg-config --libs gtk+-3.0)
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
override CFLAGS += -D_GNU_SOURCE
override CFLAGS += $(WARNINGS)
override CFLAGS += -I../include
override CFLAGS += $(BASIC_CFLAGS)
override CFLAGS += $(MODULE_DIR)
override CFLAGS += -std=gnu99

COBJECTS=$(subst .c,.o,$(FILES))

TARGET=gui-toolkit.a

all: $(COBJECTS) $(TARGET)

$(TARGET): $(COBJECTS)
	$(Q)$(do_build_static_lib)

%.o: %.c
	$(Q)$(call check_gui)

## make deps

all_objs := $(sort $(COBJECTS))
all_deps := $(all_objs:%.o=.%.d)
non_gui_deps = $(all_deps)


$(all_deps): .%.d: %.c
	$(Q)$(call check_gui_deps)

$(all_objs) : %.o : .%.d

dep_includes := $(wildcard $(non_gui_deps))

ifneq ($(dep_includes),)
 include $(dep_includes)
endif


do_clean =						\
	($(print_rm)				\
	$(RM) *.o *.so $(TARGET) core* \
	$(RM) .*.d)

clean:
	$(Q)$(do_clean)

memcheck: $(TARGET)
	G_DEBUG=gc-friendly G_SLICE=always-malloc valgrind --tool=memcheck --leak-check=full --leak-resolution=high ./$(TARGET)

cscope:
	$(RM) cscope*
	find .. -name '*.[ch]' | cscope -b -q


PHONY += force
force:

# Declare the contents of the .PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.
.PHONY: $(PHONY)
