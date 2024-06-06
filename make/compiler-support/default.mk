CFLAG_ARCH=-march=$(or $(strip $(1)),native)
CFLAG_OPT_DEBUG=-Og -g
CFLAG_OPT_GENERIC=-O2
CFLAG_OPT_SIZE=-Oz
CFLAG_OPT_SPEED=-Os
CFLAG_PEDANTIC=-Wpedantic
CFLAG_STD=-std=$(1)
CFLAG_UNHOSTED=-ffreestanding -nostdlib
CFLAG_WALL=-Wall -Wextra -Wpedantic
# TODO (dargueta): Add -pedantic-errors except for TCC
CFLAG_WERROR=-Werror
OBJECT_EXT=o
