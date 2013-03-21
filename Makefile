include project.mk

SUBDIRS  = gui-toolkit
SUBDIRS += modules/hello-world
SUBDIRS += core
SUBDIRS += data


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

install: all
	test -d $(prefix) || mkdir --parents $(prefix)
	test -d $(prefix)/bin || mkdir --parents $(prefix)/bin
	test -d $(prefix)/share || mkdir --parents $(prefix)/share
	test -d $(prefix)/share/perf-studio || mkdir --parents $(prefix)/share/perf-studio

	$(INSTALL) -m 0755 scripts/perf-studio-ctrl.py $(prefix)/share/perf-studio
	$(RM) $(prefix)/bin/perf-studio-ctrl
	ln -s $(prefix)/share/perf-studio/perf-studio-ctrl.py $(prefix)/bin/perf-studio-ctrl

	@for dir in data core; do \
					(cd $$dir && $(MAKE) install) \
	done

cscope:
	$(Q)$(do_local_clean)
	find . -name '*.[ch]' > cscope.files
	cscope -b -q

memcheck: all
	G_DEBUG=gc-friendly G_SLICE=always-malloc valgrind --tool=memcheck --leak-check=full --leak-resolution=high core/perf-studio
