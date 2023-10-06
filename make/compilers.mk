include os.mk

TCC_VERSION = 0.9.27
SDCC_VERSION = 4.3.0
GPUTILS_VERSION = 1.5.2

TCC_URL = http://download-mirror.savannah.gnu.org/releases/tinycc/tcc-$(TCC_VERSION).tar.bz2
PCC_URL = http://pcc.ludd.ltu.se/ftp/pub/pcc/pcc-current.tgz
SDCC_URL = https://versaweb.dl.sourceforge.net/project/sdcc/sdcc/$(SDCC_VERSION)/sdcc-src-$(SDCC_VERSION).tar.bz2
GPUTILS_URL = https://cfhcable.dl.sourceforge.net/project/gputils/gputils/1.5.0/gputils-$(GPUTILS_VERSION).tar.bz2
PREFIX = /usr/local
TCC_BIN = $(PREFIX)/bin/tcc
PCC_BIN = $(PREFIX)/bin/pcc
SDCC_BIN = $(PREFIX)/bin/sdcc
GPUTILS_DIR = $(PREFIX)/share/gputils

DOWNLOAD_DIR_BASE = /tmp/libmetaldata_install_downloads
TCC_DOWNLOAD_DIR = $(DOWNLOAD_DIR_BASE)/tcc
PCC_DOWNLOAD_DIR = $(DOWNLOAD_DIR_BASE)/pcc
SDCC_DOWNLOAD_DIR = $(DOWNLOAD_DIR_BASE)/sdcc
GPUTILS_DOWNLOAD_DIR = $(DOWNLOAD_DIR_BASE)/gputils

# Systems that don't have sudo, like Cygwin, will have an empty string. We use
# /usr/bin/which to avoid shell builtins that may emit an error message when a
# command isn't found. In particular, `sh` on macOS is an alias for `zsh`, which
# *does* have a shell builtin.
SUDO = $(shell /usr/bin/which sudo)

UNAME = $(shell uname -s)

ifneq ($(findstring $(UNAME),CYGWIN),)
    SDCC_CONFIG_COMMAND = ./support/scripts/sdcc_cygwin_mingw32
else
    SDCC_CONFIG_COMMAND = ./configure --prefix=$(PREFIX)
endif


.PHONY: install-tcc
install-tcc: $(TCC_BIN)
	$(RM) -r $(TCC_DOWNLOAD_DIR)


.PHONY: install-pcc
install-pcc: $(PCC_BIN)
	$(RM) -r $(PCC_DOWNLOAD_DIR)


.PHONY: install-sdcc
install-sdcc: $(SDCC_BIN)
	$(RM) -r $(SDCC_DOWNLOAD_DIR)


.PHONY: install-gputils
install-gputils: $(GPUTILS_DIR)
	$(RM) -r $(GPUTILS_DOWNLOAD_DIR)


$(GPUTILS_DIR): $(GPUTILS_DOWNLOAD_DIR)
	cd $< && curl -sS $(GPUTILS_URL) | tar -xj --strip-components=1
	cd $< && ./configure $(CONFIGURE_ARGS)
	$(MAKE) -C $<
	$(SUDO) $(MAKE) -C $< install


$(TCC_BIN): $(TCC_DOWNLOAD_DIR)
	cd $< && curl -sS $(TCC_URL) | tar -xj --strip-components=1
	cd $< && ./configure --prefix=$(PREFIX) $(CONFIGURE_ARGS)
	$(MAKE) -C $<
	$(SUDO) $(MAKE) -C $< install


$(PCC_BIN): $(PCC_DOWNLOAD_DIR)
	cd $< && curl -sS $(PCC_URL) | tar -xz --strip-components=1
	cd $< && ./configure --prefix=$(PREFIX) $(CONFIGURE_ARGS)
	$(MAKE) -C $<
	$(SUDO) $(MAKE) -C $< install


$(SDCC_BIN): $(SDCC_DOWNLOAD_DIR) $(GPUTILS_DIR)
	cd $< && curl -sS $(SDCC_URL) | tar -xj --strip-components=1
	cd $< && $(SDCC_CONFIG_COMMAND) --prefix=$(PREFIX) $(CONFIGURE_ARGS)
	$(MAKE) -C $<
	$(SUDO) $(MAKE) -C $< install


.PHONY: show-env
show-env:
	@echo 'HEADER SEARCH PATHS ------------------------------------------------'
	-gcc -v -xc++ /dev/null -fsyntax-only
	@echo 'STDLIB LOCATIONS ---------------------------------------------------'
	-find /usr -name 'stddef.h' -type f


$(TCC_DOWNLOAD_DIR) $(PCC_DOWNLOAD_DIR) $(SDCC_DOWNLOAD_DIR) $(GPUTILS_DOWNLOAD_DIR):
	mkdir -p $@
