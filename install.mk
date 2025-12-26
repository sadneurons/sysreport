PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man/man1

install: build/main sysreport.1
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 build/main $(DESTDIR)$(BINDIR)/sysreport
	install -d $(DESTDIR)$(MANDIR)
	install -m 644 sysreport.1 $(DESTDIR)$(MANDIR)/sysreport.1
	gzip -f $(DESTDIR)$(MANDIR)/sysreport.1

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/sysreport
	rm -f $(DESTDIR)$(MANDIR)/sysreport.1.gz
