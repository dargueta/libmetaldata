include os.mk

TCC_VERSION=0.9.27
SDCC_VERSION=4.4.0
PREFIX=/usr/local
CONFIGURE_ARGS =

TCC_URL=http://download-mirror.savannah.gnu.org/releases/tinycc/tcc-$(TCC_VERSION).tar.bz2
PCC_URL=http://pcc.ludd.ltu.se/ftp/pub/pcc/pcc-current.tgz
SDCC_URL_LINUX_AMD=https://sourceforge.net/projects/sdcc/files/sdcc-linux-amd64/$(SDCC_VERSION)/sdcc-$(SDCC_VERSION)-amd64-unknown-linux2.5.tar.bz2/download
SDCC_URL_MACOS_X86=https://sourceforge.net/projects/sdcc/files/sdcc-macos-amd64/$(SDCC_VERSION)/sdcc-$(SDCC_VERSION)-x86_64-apple-macosx.tar.bz2/download
TCC_BIN=$(PREFIX)/bin/tcc
PCC_BIN=$(PREFIX)/bin/pcc
SDCC_BIN=$(PREFIX)/bin/sdcc

DOWNLOAD_DIR_BASE=/tmp/libmetaldata_install_downloads
TCC_DOWNLOAD_DIR=$(DOWNLOAD_DIR_BASE)/tcc
PCC_DOWNLOAD_DIR=$(DOWNLOAD_DIR_BASE)/pcc

# Systems that don't have sudo, like Cygwin, will have an empty string. We use
# /usr/bin/which to avoid shell builtins that may emit an error message when a
# command isn't found. In particular, `sh` on macOS is an alias for `zsh`, which
# *does* have a shell builtin.
SUDO=$(shell /usr/bin/which sudo)

UNAME=$(shell uname -s)

# SDCC only supports x86 on MacOS. If you have Apple silicon you'll have to
# either use Rosetta or build from source. My laptop takes ~30-40m to build it,
# though granted my laptop is about 7 years old as I'm writing this.
ifeq ($(UNAME),Darwin)
    SDCC_URL=$(SDCC_URL_MACOS_X86)
else
    SDCC_URL=$(SDCC_URL_LINUX_AMD)
endif


.PHONY: install-tcc
install-tcc: $(TCC_BIN)
	$(RM) -r $(TCC_DOWNLOAD_DIR)


.PHONY: install-pcc
install-pcc: $(PCC_BIN)
	$(RM) -r $(PCC_DOWNLOAD_DIR)


.PHONY: install-sdcc
install-sdcc: $(SDCC_BIN)


$(TCC_BIN): $(TCC_DOWNLOAD_DIR) | $(PREFIX)
	curl -sSL $(TCC_URL) | tar -C '$<' -xj --strip-components=1
	cd $< && ./configure --prefix=$(PREFIX) $(CONFIGURE_ARGS)
	$(MAKE) -C $<
	$(SUDO) $(MAKE) -C $< install


$(PCC_BIN): $(PCC_DOWNLOAD_DIR) | $(PREFIX)
	curl -sSL $(PCC_URL) | tar -C '$<' -xz --strip-components=1
	cd $< && ./configure --prefix=$(PREFIX) $(CONFIGURE_ARGS)
	$(MAKE) -C $<
	$(SUDO) $(MAKE) -C $< install


$(SDCC_BIN): | $(PREFIX)
	curl -sSL $(SDCC_URL) | $(SUDO) tar -C $(PREFIX) -xj --strip-components=1 --exclude=INSTALL.txt --exclude=README.txt


.PHONY: show-env
show-env:
	@echo 'HEADER SEARCH PATHS ------------------------------------------------'
	-gcc -v -xc++ /dev/null -fsyntax-only
	@echo 'STDLIB LOCATIONS ---------------------------------------------------'
	-find /usr -name 'stddef.h' -type f


$(TCC_DOWNLOAD_DIR) $(PCC_DOWNLOAD_DIR) $(PREFIX):
	mkdir -p $@
