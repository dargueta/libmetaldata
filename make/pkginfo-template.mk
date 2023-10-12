define PKGINFO_TEXT
Name: $(LIB_NAME_STEM)
Description: Data structures and abstractions for low-level software
URL: https://github.com/dargueta/libmetaldata
Version: $(VERSION_FULL)
Cflags: $(PUBLIC_COMPILE_FLAGS) -I$(INSTALL_TARGET_INCLUDE)
Libs: -L$(INSTALL_TARGET_LIB)
endef
