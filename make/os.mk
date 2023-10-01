# Cygwin provides this, other platforms may not.
ifndef OS
    OS=$(shell uname -s)
endif

UNAME = $(shell uname -s)

# Set default values for all variables. These values were chosen for Linux, with
# the exception of compiler flags and disallowing versioned libraries by default.
STATIC_LIB_EXT=a
DYNAMIC_LIB_EXT=so
LIB_NAME_PREFIX=lib
OBJECT_FILE_EXT=o
DEFAULT_PREFIX=/usr/local
SHARED_LIB_FLAG=
FREESTANDING_FLAG=
ALLOW_VERSIONED_DYNAMIC_LIBRARY=0

IS_CYGWIN=$(if $(strip $(findstring $(UNAME),CYGWIN_NT)),1,0)
IS_WINDOWS=$(if $(strip $(findstring $(OS),Windows_NT)),1,0)

ifeq ($(UNAME),Linux)
    SHARED_LIB_FLAG=-shared
    FREESTANDING_FLAG=-ffreestanding
    ALLOW_VERSIONED_DYNAMIC_LIBRARY=1
else ifeq ($(UNAME),Darwin)
    DYNAMIC_LIB_EXT=dylib
    DEFAULT_PREFIX=/usr
    SHARED_LIB_FLAG=-undefined dynamic_lookup
    FREESTANDING_FLAG=-ffreestanding
    # ALLOW_VERSIONED_DYNAMIC_LIBRARY is 1 for .so, 0 for .dylib. I think.
else ifeq ($(IS_CYGWIN),1)
    SHARED_LIB_FLAG=-shared
    FREESTANDING_FLAG=-ffreestanding
    ALLOW_VERSIONED_DYNAMIC_LIBRARY=1
else ifeq ($(IS_WINDOWS),1)
    STATIC_LIB_EXT=lib
    DYNAMIC_LIB_EXT=dll
    LIB_NAME_PREFIX=
    OBJECT_FILE_EXT=obj
    DEFAULT_PREFIX=C:\\Windows\\System32
else
    $(error Unsupported operating system: OS='$(OS)' UNAME='$(UNAME)')
endif
