CD = cd
MAKEDIR = mkdir

ifeq ($(OS),Windows_NT)
    REMOVEDIR = rmdir /s
    CMAKE = cmake -G "NMake makefiles"
    MAKE = nmake
else
    REMOVEDIR = rm -rf
    CMAKE = cmake
    MAKE = make
    export CC = clang
endif

all : library

library : clean 
	$(MAKEDIR) build
	$(CD) build && $(CMAKE) .. && $(MAKE)

clean :
	$(REMOVEDIR) build
