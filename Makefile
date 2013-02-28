include global.mak

SUBDIRS = modules/bar core


.PHONY: $(SUBDIRS)
.PHONY: subdirs
.PHONY: all

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@


all: $(SUBDIRS)

clean: 
	@for dir in ${SUBDIRS}; do \
					(cd $$dir && $(MAKE) clean) \
	done

install:
	test -d $(PREFIX) || mkdir --parents $(PREFIX)
	test -d $(PREFIX)/bin || mkdir --parents $(PREFIX)/bin
	test -d $(PREFIX)/share || mkdir --parents $(PREFIX)/share
	test -d $(PREFIX)/share/perf-studio || mkdir --parents $(PREFIX)/share/perf-studio

	$(INSTALL) -m 0755 scripts/perf-studio-ctrl.py $(PREFIX)/share/perf-studio
	$(RM) $(PREFIX)/bin/perf-studio-ctrl
	ln -s $(PREFIX)/share/perf-studio/perf-studio-ctrl.py $(PREFIX)/bin/perf-studio-ctrl
