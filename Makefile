include global.mak

SUBDIRS  = gui-toolkit
SUBDIRS += modules/hello-world
SUBDIRS += core


.PHONY: $(SUBDIRS)
.PHONY: subdirs
.PHONY: all

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@


all: $(SUBDIRS)

do_local_clean =						\
	($(print_rm)				\
	$(RM) cscope*)

clean: 
	@for dir in ${SUBDIRS}; do \
					(cd $$dir && $(MAKE) clean) \
	done
	$(Q)$(do_local_clean)

install:
	test -d $(prefix) || mkdir --parents $(prefix)
	test -d $(prefix)/bin || mkdir --parents $(prefix)/bin
	test -d $(prefix)/share || mkdir --parents $(prefix)/share
	test -d $(prefix)/share/perf-studio || mkdir --parents $(prefix)/share/perf-studio

	$(INSTALL) -m 0755 scripts/perf-studio-ctrl.py $(prefix)/share/perf-studio
	$(RM) $(prefix)/bin/perf-studio-ctrl
	ln -s $(prefix)/share/perf-studio/perf-studio-ctrl.py $(prefix)/bin/perf-studio-ctrl

cscope:
	$(Q)$(do_local_clean)
	find . -name '*.[ch]' > cscope.files
	cscope -b -q -k

