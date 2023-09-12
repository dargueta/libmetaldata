# Cygwin provides this, other platforms may not.
ifndef OS
    OS=$(shell uname -s)
endif

# Set default values for all variables. These values were chosen for Linux, with
# the exception of compiler flags and disallowing versioned libraries by default.
STATIC_LIB_EXT=a
DYNAMIC_LIB_EXT=so
LIB_NAME_PREFIX=lib
OBJECT_FILE_EXT=o
DEFAULT_INSTALL_TOP=/usr/local
SHARED_LIB_FLAG=
FREESTANDING_FLAG=
ALLOW_VERSIONED_DYNAMIC_LIBRARY=0

IS_WINDOWS=$(strip $(or $(findstring $(OS),Windows_NT) $(findstring $(OS),CYGWIN_NT)))

ifneq ($(IS_WINDOWS),)
    STATIC_LIB_EXT=lib
    DYNAMIC_LIB_EXT=dll
    LIB_NAME_PREFIX=
    OBJECT_FILE_EXT=obj
    DEFAULT_INSTALL_TOP=C:\\Windows\\System32
else ifeq ($(OS),Darwin)
    DYNAMIC_LIB_EXT=dylib
    DEFAULT_INSTALL_TOP=/usr
    SHARED_LIB_FLAG=-undefined dynamic_lookup
    FREESTANDING_FLAG=-ffreestanding -nostdlib
    # ALLOW_VERSIONED_DYNAMIC_LIBRARY is 1 for .so, 0 for .dylib. I think.
else ifeq ($(OS),Linux)
    SHARED_LIB_FLAG=-shared
    FREESTANDING_FLAG=-ffreestanding -nostdlib
    ALLOW_VERSIONED_DYNAMIC_LIBRARY=1
else
    $(error Unsupported operating system: $(OS))
endif
