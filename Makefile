include make/os.mk
include Makefile.in

MY_CFLAGS=
MY_LDFLAGS=

# If you don't have the install command on your system, you may need to change
# this to something else.
CMD_INSTALL=install -m644
CMD_INSTALL_BIN=install -m755

# End users shouldn't need to modify anything below this line. =================
SOURCE_ROOT=src
HEADER_ROOT=$(SOURCE_ROOT)/metaldata

# Source stuff
LIBRARY_C_SOURCE_FILES=$(wildcard $(SOURCE_ROOT)/*.c)
LIBRARY_C_PUBLIC_HEADERS=$(wildcard $(HEADER_ROOT)/*.h)
LIBRARY_C_PRIVATE_HEADERS=$(wildcard $(HEADER_ROOT)/internal/*.h)
LIBRARY_C_ALL_HEADER_FILES=$(LIBRARY_C_PUBLIC_HEADERS) $(LIBRARY_C_PRIVATE_HEADERS)
LIBRARY_OBJECT_FILES=$(LIBRARY_C_SOURCE_FILES:%.c=%.$(OBJECT_FILE_EXT))
LIBRARY_EXTRAS=$(wildcard $(HEADER_ROOT)/extras/*.c) $(wildcard $(HEADER_ROOT)/extras/*.h)

TEST_C_SOURCE_FILES=$(wildcard tests/*.c)
TEST_C_HEADERS=$(wildcard tests/*.h)
TEST_OBJECT_FILES=$(TEST_C_SOURCE_FILES:%.c=%.$(OBJECT_FILE_EXT)) tests/munit/munit.$(OBJECT_FILE_EXT)

# Build targets and such
BUILD_DIR=build
LIB_NAME_STEM=$(LIB_NAME_PREFIX)metaldata
STATIC_LIBRARY=$(BUILD_DIR)/$(LIB_NAME_STEM).$(STATIC_LIB_EXT)
DYNAMIC_LIBRARY=$(BUILD_DIR)/$(LIB_NAME_STEM).$(DYNAMIC_LIB_EXT)
VERSIONED_DYNAMIC_LIBRARY=$(DYNAMIC_LIBRARY).$(VERSION_MAJOR)
TEST_BINARY=$(BUILD_DIR)/test

ALL_OBJECT_FILES = $(LIBRARY_OBJECT_FILES) $(TEST_OBJECT_FILES)

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

C_STANDARD = c99
ARCH_FLAG =

ifeq ($(notdir $(CC)),sdcc)
	include make/compiler-support/sdcc.mk
else ifeq ($(USE_MINIMAL_FLAGS),1)
	include make/compiler-support/minimal.mk
else
	include make/compiler-support/default.mk
endif

ifeq ($(DEBUG_MODE),0)
    CFLAGS_OPTIMIZATION = $(GENERIC_OPTIMIZATION_FLAG)
else
    CFLAGS_OPTIMIZATION = $(GENERIC_OPTIMIZATION_FLAG) $(DEBUG_SYMBOLS_FLAG)
endif

ifeq ($(UNHOSTED_IMPLEMENTATION),1)
    CFLAGS_FREESTANDING = $(FREESTANDING_FLAG) $(NOSTDLIB_FLAG)
else
	CFLAGS_FREESTANDING=
endif

ifneq ($(USE_MINIMAL_FLAGS),0)
    BUILD_WARNING_FLAGS =
    TEST_WARNING_FLAGS =
else
    ifeq ($(NO_FATAL_WARNINGS),1)
        # No fatal warnings at all
        BUILD_WARNING_FLAGS = $(BASE_WARNING_FLAGS)
        TEST_WARNING_FLAGS = $(BUILD_WARNING_FLAGS)
    else
        # Warnings are fatal except when building tests.
        BUILD_WARNING_FLAGS = $(BASE_WARNING_FLAGS) $(WERROR_FLAG)
        TEST_WARNING_FLAGS = $(BASE_WARNING_FLAGS)
    endif
endif

# These are the compilation flags that should be exposed to anyone trying to link
# with the library. Custom macro definitions are one example.
PUBLIC_COMPILE_FLAGS=$(strip $(MACROS_FROM_CONFIGURE))

# This is the minimal set of flags needed to compile the library. It's
ADDL_CFLAGS_MINIMAL = -I./src $(PUBLIC_COMPILE_FLAGS) $(MY_CFLAGS)
ADDL_CFLAGS_FULL = $(ADDL_CFLAGS_MINIMAL) $(C_STANDARD_FLAG) $(CFLAGS_OPTIMIZATION) $(ARCH_FLAG)

ifeq ($(USE_MINIMAL_FLAGS),0)
    COMPILE_COMMAND=$(CC) $(CFLAGS) $(ADDL_CFLAGS_FULL) -c
else
    COMPILE_COMMAND=$(CC) $(CFLAGS) $(ADDL_CFLAGS_MINIMAL) -c
endif

INSTALL_BINARY = $(if $1,mkdir -p $2 && $(CMD_INSTALL_BIN) $1 $2)
INSTALL_BINARY_WILDCARD = $(call INSTALL_BINARY,$(wildcard $1),$2)
INSTALL_FILE = $(if $1,mkdir -p $2 && $(CMD_INSTALL) $1 $2)
INSTALL_FILE_WILDCARD = $(call INSTALL_FILE,$(wildcard $1),$2)
INSTALL_RECURSIVE = mkdir -p $2 && cp -r $1/. $2
DOC_INDEX_FILE = documentation/api/html/index.html

# This must be included only after all variables are defined.
include make/pkginfo-template.mk
include make/configuration-header.mk

.PHONY: __in_debug_mode all clean docs format header install library show_docs test

all: library $(PKGCONFIG_FILE)

install: all $(LIBRARY_C_ALL_HEADER_FILES) $(LIBRARY_EXTRAS)
	$(call INSTALL_FILE,$(STATIC_LIBRARY),$(INSTALL_TARGET_LIB))
	$(call INSTALL_RECURSIVE,$(HEADER_ROOT),$(INSTALL_TARGET_INCLUDE))
	$(call INSTALL_FILE,$(PKGCONFIG_FILE),$(INSTALL_TARGET_PKGCONFIG))

library: $(STATIC_LIBRARY)

test: $(TEST_BINARY)
	$(TEST_BINARY)

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -r $(dir $(DOC_INDEX_FILE))
	$(RM) $(ALL_OBJECT_FILES)
	$(RM) $(ALL_OBJECT_FILES:%.$(OBJECT_FILE_EXT)=%.adb)
	$(RM) $(ALL_OBJECT_FILES:%.$(OBJECT_FILE_EXT)=%.asm)
	$(RM) $(ALL_OBJECT_FILES:%.$(OBJECT_FILE_EXT)=%.lst)
	$(RM) $(ALL_OBJECT_FILES:%.$(OBJECT_FILE_EXT)=%.rel)
	$(RM) $(ALL_OBJECT_FILES:%.$(OBJECT_FILE_EXT)=%.sym)
	$(RM) $(ALL_OBJECT_FILES:%.$(OBJECT_FILE_EXT)=%.d)

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

$(DYNAMIC_LIBRARY): $(STATIC_LIBRARY)
	$(LD) $(LDFLAGS) $(SHARED_LIB_FLAG) $(MY_LDFLAGS) -o $@ $^

$(TEST_BINARY): $(TEST_OBJECT_FILES) $(STATIC_LIBRARY)
	$(CC) $(LDFLAGS) $(MY_LDFLAGS) -o $@ $^

export PKGINFO_TEXT
$(PKGCONFIG_FILE): Makefile.in make/pkginfo-template.mk | $(BUILD_DIR)
	echo "$${PKGINFO_TEXT}" > $@

header: $(CONFIG_HEADER_FILE)

export CONFIGURATION_HEADER_TEXT
$(CONFIG_HEADER_FILE): Makefile.in make/configuration-header.mk
	echo "$${CONFIGURATION_HEADER_TEXT}" | tr '`' '#' > $@

tests/%.$(OBJECT_FILE_EXT): tests/%.c $(CONFIG_HEADER_FILE) __in_debug_mode
	$(COMPILE_COMMAND) $(TEST_WARNING_FLAGS) -D MDL_CURRENTLY_COMPILING_TESTS=1 -I./tests -o $@ $<

%.$(OBJECT_FILE_EXT): %.c $(CONFIG_HEADER_FILE)
	$(COMPILE_COMMAND) $(BUILD_WARNING_FLAGS) -D MDL_CURRENTLY_COMPILING_LIBRARY=1 $(CFLAGS_FREESTANDING) -o $@ $<

$(BUILD_DIR):
	mkdir -p $@

# PEBKAC Rules -----------------------------------------------------------------
Makefile.in:
	@echo 'You must run the `configure` script before running Make.' ; exit 1

__in_debug_mode:
	@if [ "$(DEBUG_MODE)" -eq 0 ]; then \
        echo 'Recompile project in debug mode. See `configure -h` for details.'; \
        exit 1; \
    fi
