include ../project.mk

all:
	@echo -n

do_clean =						\
	($(print_rm))

clean:
	$(Q)$(do_clean)

install:
	@echo install to $(data_dir)
	test -d $(data_dir) || mkdir --parents $(data_dir)
	cp -r artwork    $(data_dir)
	cp -r help-pages $(data_dir)

uninstall:
	$(RM) -rf $(data_dir)

.PHONY: install uninstall clean all
