MAKEFLAGS += --no-print-directory
PREFIX?=/usr

SUBDIRS = modules core

INSTALL = /usr/bin/install -c -m 0755
INSTALLDATA = /usr/bin/install -c -m 0644

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
	$(INSTALL) -m 0755 scripts/perf-studio-ctrl.py $(PREFIX)/share/perf-studio
	$(RM) $(PREFIX)/bin/captcp
	ln -s $(PREFIX)/share/perf-studio/perf-studio-ctrl.py $(PREFIX)/bin/perf-studio-ctrl
