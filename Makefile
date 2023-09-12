include make/os.mk
include Makefile.in

MY_CFLAGS=
MY_LDFLAGS=

# If you don't have the install command on your system, you may need to change
# this to something else.
INSTALL=install -vpD -m 0644

# End users shouldn't need to modify anything below this line. =================

# Source stuff
LIBRARY_C_SOURCE_FILES=$(wildcard src/*.c)
LIBRARY_C_PUBLIC_HEADERS=$(wildcard src/metaldata/*.h)
LIBRARY_C_PRIVATE_HEADERS=$(wildcard src/metaldata/internal/*.h)
LIBRARY_C_ALL_HEADER_FILES=$(LIBRARY_C_PUBLIC_HEADERS) $(LIBRARY_C_PRIVATE_HEADERS)
LIBRARY_OBJECT_FILES=$(LIBRARY_C_SOURCE_FILES:%.c=%.$(OBJECT_FILE_EXT))

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

SYMBOL_FILES=$(LIBRARY_OBJECT_FILES:%.$(OBJECT_FILE_EXT)=%.sym) $(TEST_OBJECT_FILES:%.$(OBJECT_FILE_EXT)=%.sym)
LISTING_FILES=$(LIBRARY_OBJECT_FILES:%.$(OBJECT_FILE_EXT)=%.lst) $(TEST_OBJECT_FILES:%.$(OBJECT_FILE_EXT)=%.lst)

# Stuff to generate
CONFIG_HEADER_FILE=src/metaldata/configuration.h
PKGCONFIG_FILE=$(BUILD_DIR)/$(LIB_NAME_STEM).pc


ifndef INSTALL_TOP
    INSTALL_TOP=$(DEFAULT_INSTALL_TOP)
endif

INSTALL_LIB=$(INSTALL_TOP)/lib
INSTALL_INCLUDE=$(INSTALL_TOP)/include
INSTALL_BIN=$(INSTALL_TOP)/bin
INSTALL_PKGCONFIG=$(INSTALL_LIB)/pkgconfig


ifneq ($(DEBUG_MODE),0)
    CFLAGS_OPTIMIZATION=-O0 -g
else
    CFLAGS_OPTIMIZATION=-O2
endif

ifeq ($(UNHOSTED_IMPLEMENTATION),1)
	CFLAGS_FREESTANDING=$(FREESTANDING_FLAG)
else
	CFLAGS_FREESTANDING=
endif

ifneq ($(NO_FATAL_WARNINGS),0)
    WARNING_FLAGS=-Wall -Wextra
else
    WARNING_FLAGS=-Wall -Wextra -Werror
endif

# For each feature macro named X, generate a CLI argument "-DX=$(X)" that we can
# pass to the compiler.
FEATURE_MACRO_FLAGS_DEFINITIONS=$(foreach n,$(FEATURE_MACRO_FLAG_NAMES),-D$(n)=$($(n)))

# These are the compilation flags that should be exposed to anyone trying to link
# with the library. Custom macro definitions are one example.
PUBLIC_COMPILE_FLAGS=$(strip $(MACROS_FROM_CONFIGURE) $(FEATURE_MACRO_FLAGS_DEFINITIONS))

# This is the minimal set of flags needed to compile the library. It's
ADDL_CFLAGS_MINIMAL=-I./src $(PUBLIC_COMPILE_FLAGS) $(MY_CFLAGS)
ADDL_CFLAGS_FULL=$(ADDL_CFLAGS_MINIMAL) $(WARNING_FLAGS) -std=c99 $(CFLAGS_OPTIMIZATION)

ifeq ($(USE_MINIMAL_FLAGS),0)
    COMPILE_COMMAND=$(CC) $(CFLAGS) $(ADDL_CFLAGS_FULL) -c
else
    COMPILE_COMMAND=$(CC) $(CFLAGS) $(ADDL_CFLAGS_MINIMAL) -c
endif

# This must be included only after all variables are defined.
include make/pkginfo-template.mk

.PHONY: all
all: library $(PKGCONFIG_FILE)

.PHONY: install
install: all $(LIBRARY_C_ALL_HEADER_FILES)
	$(INSTALL) -t $(INSTALL_LIB) $(STATIC_LIBRARY)
	$(INSTALL) -t $(INSTALL_INCLUDE)/metaldata $(LIBRARY_C_PUBLIC_HEADERS)
	$(INSTALL) -t $(INSTALL_INCLUDE)/metaldata/internal $(LIBRARY_C_PRIVATE_HEADERS)
	$(INSTALL) -t $(INSTALL_PKGCONFIG) $(PKGCONFIG_FILE)

.PHONY: library
library: $(STATIC_LIBRARY)

.PHONY: test
test: $(TEST_BINARY)
	./$(TEST_BINARY)

.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) $(LIBRARY_OBJECT_FILES) $(TEST_OBJECT_FILES)
	$(RM) $(SYMBOL_FILES)
	$(RM) $(LISTING_FILES)

.PHONY: format
format: $(LIBRARY_C_SOURCE_FILES) $(LIBRARY_C_ALL_HEADER_FILES) \
        $(TEST_C_SOURCE_FILES) $(TEST_C_HEADERS)
	clang-format --verbose --style=file -i --Werror $^

$(STATIC_LIBRARY): $(LIBRARY_OBJECT_FILES) | $(BUILD_DIR)
	$(AR) rcus $@ $^

$(DYNAMIC_LIBRARY): $(STATIC_LIBRARY)
	$(LD) $(LDFLAGS) $(SHARED_LIB_FLAG) $(MY_LDFLAGS) -o $@ $^

$(TEST_BINARY): $(TEST_OBJECT_FILES) $(STATIC_LIBRARY)
	$(CC) $(LDFLAGS) $(MY_LDFLAGS) -o $@ $^

export PKGINFO_TEXT
$(PKGCONFIG_FILE): Makefile.in | $(BUILD_DIR)
	echo "$${PKGINFO_TEXT}" > $@

%.o: %.c $(CONFIG_HEADER_FILE)
	$(COMPILE_COMMAND) -D MDL_CURRENTLY_COMPILING_LIBRARY=1 $(CFLAGS_FREESTANDING) -o $@ $<

tests/%.o: tests/%.c $(CONFIG_HEADER_FILE) __in_debug_mode
	$(COMPILE_COMMAND) -D MDL_CURRENTLY_COMPILING_TESTS=1 -I./tests -o $@ $<

$(BUILD_DIR):
	mkdir -p $@

# PEBKAC Rules -----------------------------------------------------------------
Makefile.in $(CONFIG_HEADER_FILE):
	@echo 'You must run the `configure` script before running Make.' ; exit 1

.PHONY: __in_debug_mode
__in_debug_mode:
	@if [ "$(DEBUG_MODE)" -eq 0 ]; then \
        echo 'Recompile project in debug mode. See `configure -h` for details.'; \
        exit 1; \
    fi
