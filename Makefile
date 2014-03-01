SUBDIRS = core

.PHONY: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: all

all: $(SUBDIRS)

clean: 
	@for dir in ${SUBDIRS}; do \
					(cd $$dir && $(MAKE) clean) \
	done

