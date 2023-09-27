include ./os.mk

TCC_VERSION = 0.9.27
TCC_URL = http://download-mirror.savannah.gnu.org/releases/tinycc/tcc-$(TCC_VERSION).tar.bz2
PCC_URL = http://pcc.ludd.ltu.se/ftp/pub/pcc/pcc-current.tgz
DOWNLOAD_DIR = __libmetaldata_install_downloads__
PREFIX = /usr/local
TCC_BIN = $(PREFIX)/bin/tcc
PCC_BIN = $(PREFIX)/bin/pcc

# Systems that don't have sudo, like Cygwin, will have an empty string. We use
# /usr/bin/which to avoid shell builtins that may emit an error message when a
# command isn't found. In particular, `sh` on macOS is an alias for `zsh`, which
# *does* have a shell builtin.
SUDO = $(shell /usr/bin/which sudo)


.PHONY: install-tcc
install-tcc: $(TCC_BIN)
	$(RM) -r $(DOWNLOAD_DIR)


.PHONY: install-pcc
install-pcc: $(PCC_BIN)
	$(RM) -r $(DOWNLOAD_DIR)


$(TCC_BIN): $(DOWNLOAD_DIR)
	cd $< && curl $(TCC_URL) | tar -xj --strip-components=1
	cd $< && ./configure --prefix $(PREFIX)
	$(MAKE) -C $<
	$(SUDO) $(MAKE) -C $< install


$(PCC_BIN): $(DOWNLOAD_DIR)
	cd $< && curl $(PCC_URL) | tar -xz --strip-components=1
	cd $< && ./configure --prefix $(PREFIX)
	$(MAKE) -C $<
	$(SUDO) $(MAKE) -C $< install


.PHONY: show-env
show-env:
	@echo 'HEADER SEARCH PATHS ------------------------------------------------'
	-gcc -v -xc++ /dev/null -fsyntax-only
	@echo 'STDLIB LOCATIONS ---------------------------------------------------'
	-find /usr -name 'stddef.h' -type f


$(DOWNLOAD_DIR):
	mkdir -p $@
