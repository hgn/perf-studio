include ../../global.mak

MODULE_OBJS = hello-world.o
MODULES := $(MODULE_OBJS:.o=.so)

BASIC_CFLAGS  = $(shell pkg-config --cflags gtk+-3.0)
BASIC_CFLAGS += $(shell pkg-config --cflags dbus-1)

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
override CFLAGS += -I../../include
override CFLAGS += $(BASIC_CFLAGS)
override CFLAGS += $(MODULE_DIR)
override CFLAGS += -std=gnu99

all: $(MODULE_OBJS) $(MODULES)

$(obj)/%.o: %.c
	$(Q)$(call check_gui)

%.o: %.c
	$(Q)$(call check_gui)

$(MODULE_OBJS): %.o : %.c 
	$(Q)$(do_compile_module_obj)

$(MODULES): %.so : %.o 
	$(Q)$(do_module_build)



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
