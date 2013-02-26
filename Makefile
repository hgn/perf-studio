MAKEFLAGS += --no-print-directory

SUBDIRS = modules core

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

