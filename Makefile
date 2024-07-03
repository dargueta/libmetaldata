include make/os.mk
include Makefile.in

MY_CFLAGS=
MY_LDFLAGS=

# If you don't have the install command on your system, you may need to change
# this to something else.
CMD_INSTALL=install -m644
CMD_INSTALL_BIN=install -m755

# ============================================================================ #
#        End users shouldn't need to modify anything below this line.          #
# ============================================================================ #

ifeq ($(notdir $(CC)),sdcc)
    include make/compiler-support/sdcc.mk
else ifeq ($(USE_MINIMAL_FLAGS),1)
    include make/compiler-support/minimal.mk
else
    include make/compiler-support/default.mk
endif

SOURCE_ROOT=src
HEADER_ROOT=$(SOURCE_ROOT)/metaldata

# Source stuff
LIBRARY_C_SOURCE_FILES=$(wildcard $(SOURCE_ROOT)/*.c)
LIBRARY_C_PUBLIC_HEADERS=$(wildcard $(HEADER_ROOT)/*.h)
LIBRARY_C_PRIVATE_HEADERS=$(wildcard $(HEADER_ROOT)/internal/*.h)
LIBRARY_C_ALL_HEADER_FILES=$(LIBRARY_C_PUBLIC_HEADERS) $(LIBRARY_C_PRIVATE_HEADERS)
LIBRARY_OBJECT_FILES=$(LIBRARY_C_SOURCE_FILES:%.c=%.$(OBJECT_EXT))
LIBRARY_EXTRAS=$(wildcard $(HEADER_ROOT)/extras/*.c) $(wildcard $(HEADER_ROOT)/extras/*.h)

TEST_C_SOURCE_FILES=$(wildcard tests/*.c)
TEST_C_HEADERS=$(wildcard tests/*.h)
TEST_OBJECT_FILES=$(TEST_C_SOURCE_FILES:.c=.$(OBJECT_EXT)) tests/munit/munit.$(OBJECT_EXT)

ALL_C_SOURCE_FILES=$(LIBRARY_C_SOURCE_FILES) $(TEST_C_SOURCE_FILES)

# Build targets and such
BUILD_DIR=build
LIB_NAME_STEM=$(LIB_NAME_PREFIX)metaldata
STATIC_LIBRARY=$(BUILD_DIR)/$(LIB_NAME_STEM).$(STATIC_LIB_EXT)
TEST_BINARY=$(BUILD_DIR)/test

ALL_OBJECT_FILES=$(LIBRARY_OBJECT_FILES) $(TEST_OBJECT_FILES)

# Stuff to generate
CONFIG_HEADER_FILE=src/metaldata/configuration.h
PKGCONFIG_FILE=$(BUILD_DIR)/$(LIB_NAME_STEM).pc


ifndef PREFIX
    PREFIX=$(DEFAULT_PREFIX)
endif

INSTALL_TARGET_LIB=$(PREFIX)/lib
INSTALL_TARGET_INCLUDE=$(PREFIX)/include/metaldata
INSTALL_TARGET_BIN=$(PREFIX)/bin
INSTALL_TARGET_PKGCONFIG=$(INSTALL_TARGET_LIB)/pkgconfig

C_STANDARD=c99

ifeq ($(DEBUG_MODE),0)
    OPTIMIZING_FLAGS=$(CFLAG_OPT_GENERIC)
else
    OPTIMIZING_FLAGS=$(CFLAG_OPT_DEBUG)
endif

ifeq ($(UNHOSTED_IMPLEMENTATION),1)
    UNHOSTED_FLAGS=$(CFLAG_UNHOSTED)
else
    UNHOSTED_FLAGS=
endif

ifneq ($(USE_MINIMAL_FLAGS),0)
    BUILD_WARNING_FLAGS=
    TEST_WARNING_FLAGS=
else
    ifeq ($(NO_FATAL_WARNINGS),1)
        # No fatal warnings at all
        BUILD_WARNING_FLAGS=$(CFLAG_WALL)
        TEST_WARNING_FLAGS=$(CFLAG_WALL)
    else
        # Warnings are fatal except when building tests.
        BUILD_WARNING_FLAGS=$(CFLAG_WALL) $(CFLAG_WERROR)
        TEST_WARNING_FLAGS=$(CFLAG_WALL)
    endif
endif

# These are the compilation flags that should be exposed to anyone trying to link
# with the library. Custom macro definitions are one example.
PUBLIC_COMPILE_FLAGS=$(strip $(MACROS_FROM_CONFIGURE) $(CFLAGS_FROM_CONFIGURE))
PUBLIC_LINK_FLAGS=$(strip $(LDFLAGS_FROM_CONFIGURE))

# This is the minimal set of flags needed to compile the library. It's
ADDL_CFLAGS_MINIMAL=-I./src $(PUBLIC_COMPILE_FLAGS)
ADDL_CFLAGS_FULL=$(ADDL_CFLAGS_MINIMAL) $(C_STANDARD_FLAG) $(OPTIMIZING_FLAGS) \
                 $(call CFLAG_ARCH,$(TARGET_ARCHITECTURE))

# MY_CFLAGS must come at the end
ifeq ($(USE_MINIMAL_FLAGS),0)
    COMPILE_COMMAND=$(CC) $(CFLAGS) $(ADDL_CFLAGS_FULL) $(MY_CFLAGS) -c
else
    COMPILE_COMMAND=$(CC) $(CFLAGS) $(ADDL_CFLAGS_MINIMAL) $(MY_CFLAGS) -c
endif

do_install_binary=$(if $1,mkdir -p $2 && $(CMD_INSTALL_BIN) $1 $2)
do_install_binary_wildcard=$(call do_install_binary,$(wildcard $1),$2)
do_install_file=$(if $1,mkdir -p $2 && $(CMD_INSTALL) $1 $2)
do_install_file_wildcard=$(call do_install_file,$(wildcard $1),$2)
do_install_recursive=mkdir -p $2 && cp -r $1/. $2

DOC_INDEX_FILE=documentation/api/html/index.html

# This must be included only after all variables are defined.
include make/pkginfo-template.mk
include make/configuration-header.mk

.PHONY: all clean docs format header install library show_docs test
.DELETE_ON_ERROR: %.$(OBJECT_EXT)

library: $(STATIC_LIBRARY) $(PKGCONFIG_FILE)

all: library $(TEST_BINARY)

install: library $(LIBRARY_C_ALL_HEADER_FILES) $(LIBRARY_EXTRAS)
	$(call do_install_file,$(STATIC_LIBRARY),$(INSTALL_TARGET_LIB))
	$(call do_install_recursive,$(HEADER_ROOT),$(INSTALL_TARGET_INCLUDE))
	$(call do_install_file,$(PKGCONFIG_FILE),$(INSTALL_TARGET_PKGCONFIG))

test: $(TEST_BINARY)
	$(TEST_BINARY) $(ARGS)

# SDCC generates multiple output files per source file. When cleaning, we need to
# ensure we delete all of them.
SDCC_OTHER_GENERATED_EXTENSIONS=adb asm d ihx lst map noi rel sym
SDCC_ALL_OTHER_GENERATED_FILES=\
    $(foreach ext,\
              $(SDCC_OTHER_GENERATED_EXTENSIONS),\
              $(ALL_OBJECT_FILES:.$(OBJECT_EXT)=.$(ext)))


clean: clean-analysis
	$(RM) -r $(BUILD_DIR)
	$(RM) -r $(dir $(DOC_INDEX_FILE))
	$(RM) $(ALL_OBJECT_FILES)
	$(RM) $(SDCC_ALL_OTHER_GENERATED_FILES)

format: $(LIBRARY_C_SOURCE_FILES) $(LIBRARY_C_ALL_HEADER_FILES) \
        $(TEST_C_SOURCE_FILES) $(TEST_C_HEADERS) $(LIBRARY_EXTRAS)
	clang-format --verbose --style=file -i --Werror $^

docs: $(DOC_INDEX_FILE)

show_docs: $(DOC_INDEX_FILE)
	open $<

$(DOC_INDEX_FILE): Doxyfile $(LIBRARY_C_SOURCE_FILES) $(LIBRARY_C_ALL_HEADER_FILES)
	mkdir -p $(@D)
	doxygen

$(STATIC_LIBRARY): $(LIBRARY_OBJECT_FILES) | $(BUILD_DIR)
	$(AR) rcs $@ $^

$(TEST_BINARY): $(TEST_OBJECT_FILES) $(STATIC_LIBRARY)
	$(CC) $(LDFLAGS) $(PUBLIC_LINK_FLAGS) $(MY_LDFLAGS) -o $@ $^

export PKGINFO_TEXT
$(PKGCONFIG_FILE): Makefile.in make/pkginfo-template.mk | $(BUILD_DIR)
	echo "$${PKGINFO_TEXT}" > $@

header: $(CONFIG_HEADER_FILE)

export CONFIGURATION_HEADER_TEXT
$(CONFIG_HEADER_FILE): Makefile.in make/configuration-header.mk
	echo "$${CONFIGURATION_HEADER_TEXT}" | tr '`' '#' > $@

tests/%.$(OBJECT_EXT): tests/%.c $(CONFIG_HEADER_FILE) $(TEST_C_HEADERS)
	$(COMPILE_COMMAND) $(TEST_WARNING_FLAGS) -D MDL_CURRENTLY_COMPILING_TESTS=1 \
	    -I./tests -o $@ $(filter-out %.h,$^)

%.$(OBJECT_EXT): %.c $(CONFIG_HEADER_FILE)
	$(COMPILE_COMMAND) $(BUILD_WARNING_FLAGS) -D MDL_CURRENTLY_COMPILING_LIBRARY=1 \
	    $(UNHOSTED_FLAGS) -o $@ $<

$(BUILD_DIR):
	mkdir -p $@

# PEBKAC Rules -----------------------------------------------------------------
Makefile.in:
	@echo 'You must run the `configure` script before running Make.' ; exit 1

# Analysis ---------------------------------------------------------------------
include make/development-targets.mk
