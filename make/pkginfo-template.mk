define PKGINFO_TEXT
Name: $(LIB_NAME_STEM)
Description: Data structures for low-level software
URL: https://github.com/dagueta/libdatastructs
Version: $(VERSION_FULL)
Cflags: $(PUBLIC_COMPILE_FLAGS) -I$(INSTALL_INCLUDE)
Libs: -L$(INSTALL_LIB)
endef
