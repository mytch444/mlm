BINDIR=/usr/bin

mlm: mlm.c mlm.h file_operators.c operators.c
	gcc -Wall -o mlm mlm.c

.PHONY: install
install: mlm install-share
	install -Dm 755 mlm $(DESTDIR)$(BINDIR)/mlm

include_files=include/stdlib.l include/stdio.l

.PHONY: install-share
install-share: include/*
	install -dm 755 $(DESTDIR)/usr/share/mlm
	$(foreach p,$(include_files), $(shell install -Dm 644 $(p) $(DESTDIR)/usr/share/mlm/`basename $(p)`))
