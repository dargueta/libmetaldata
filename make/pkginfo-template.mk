define PKGINFO_TEXT
Name: $(LIB_NAME_STEM)
Description: Data structures and abstractions for low-level software
URL: https://github.com/dargueta/libmetaldata
Version: $(VERSION_FULL)
Cflags: $(strip $(PUBLIC_COMPILE_FLAGS) -I$(PREFIX)/include)
Libs: -L$(PREFIX)/lib -lmetaldata
endef
