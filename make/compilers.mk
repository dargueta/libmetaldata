include os.mk

TCC_VERSION = 0.9.27
SDCC_VERSION = 4.3.0

TCC_URL = http://download-mirror.savannah.gnu.org/releases/tinycc/tcc-$(TCC_VERSION).tar.bz2
PCC_URL = http://pcc.ludd.ltu.se/ftp/pub/pcc/pcc-current.tgz
SDCC_URL = https://versaweb.dl.sourceforge.net/project/sdcc/sdcc/$(SDCC_VERSION)/sdcc-src-$(SDCC_VERSION).tar.bz2
DOWNLOAD_DIR = __libmetaldata_install_downloads__
PREFIX = /usr/local
TCC_BIN = $(PREFIX)/bin/tcc
PCC_BIN = $(PREFIX)/bin/pcc
SDCC_BIN = $(PREFIX)/bin/sdcc

# Systems that don't have sudo, like Cygwin, will have an empty string. We use
# /usr/bin/which to avoid shell builtins that may emit an error message when a
# command isn't found. In particular, `sh` on macOS is an alias for `zsh`, which
# *does* have a shell builtin.
SUDO = $(shell /usr/bin/which sudo)

UNIX_BUILD = \
    cd $1 && ./configure --prefix $(PREFIX) $(CONFIGURE_ARGS) \
    $(MAKE) -C $1 \
    $(SUDO) $(MAKE) -C $1 install


.PHONY: install-tcc
install-tcc: $(TCC_BIN)
	$(RM) -r $(DOWNLOAD_DIR)


.PHONY: install-pcc
install-pcc: $(PCC_BIN)
	$(RM) -r $(DOWNLOAD_DIR)


.PHONY: install-sdcc
install-sdcc: $(SDCC_BIN)
	$(RM) -r $(DOWNLOAD_DIR)


$(TCC_BIN): $(DOWNLOAD_DIR)
	cd $< && curl $(TCC_URL) | tar -xj --strip-components=1
	$(call UNIX_BUILD,$<)


$(PCC_BIN): $(DOWNLOAD_DIR)
	cd $< && curl $(PCC_URL) | tar -xz --strip-components=1
	$(call UNIX_BUILD,$<)


$(SDCC_BIN): $(DOWNLOAD_DIR)
	cd $< && curl $(SDCC_URL) | tar -xj --strip-components=1
	$(call UNIX_BUILD,$<)


.PHONY: show-env
show-env:
	@echo 'HEADER SEARCH PATHS ------------------------------------------------'
	-gcc -v -xc++ /dev/null -fsyntax-only
	@echo 'STDLIB LOCATIONS ---------------------------------------------------'
	-find /usr -name 'stddef.h' -type f


$(DOWNLOAD_DIR):
	mkdir -p $@
