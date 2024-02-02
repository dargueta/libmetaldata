# Cygwin provides this, other platforms may not.
ifndef OS
    OS=$(shell uname -s)
endif

UNAME = $(shell uname -s)

# Set default values for all variables. These values were chosen for Linux, with
# the exception of compiler flags and disallowing versioned libraries by default.
STATIC_LIB_EXT=a
LIB_NAME_PREFIX=lib
OBJECT_FILE_EXT=o
DEFAULT_PREFIX=/usr/local
FREESTANDING_FLAG=

IS_CYGWIN=$(if $(strip $(findstring $(UNAME),CYGWIN_NT)),1,0)
IS_WINDOWS=$(if $(strip $(findstring $(OS),Windows_NT)),1,0)

ifeq ($(UNAME),Linux)
    FREESTANDING_FLAG=-ffreestanding
else ifeq ($(UNAME),Darwin)
    DEFAULT_PREFIX=/usr
    FREESTANDING_FLAG=-ffreestanding
else ifeq ($(IS_CYGWIN),1)
    FREESTANDING_FLAG=-ffreestanding
else ifeq ($(IS_WINDOWS),1)
    STATIC_LIB_EXT=lib
    LIB_NAME_PREFIX=
    OBJECT_FILE_EXT=obj
    DEFAULT_PREFIX=C:\\Windows\\System32
else
    $(error Unsupported operating system: OS='$(OS)' UNAME='$(UNAME)')
endif
