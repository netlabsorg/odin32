###############################################################################
# GNUmakefile :: Build the PmApiTest Program                                  #
# --------------------------------------------------------------------------- #
# For this initial version we start with a PM version because, well, we are   #
# on OS/2! However, this is not the module of focus since it is not directl   #
# connected to the SWT for OS/2 project. Upcoming commits will bring the      #
# Odin32 based and Native Win32 variants.                                     #
###############################################################################

# This is the default target when no targets are specified.
.DEFAULT_GOAL:=default

# Use the wl.exe Open Watcom derivative for linking
export EMXOMFLD_TYPE=wlink
export EMXOMFLD_LINKER=wl.exe

# The header, source and output directories relative to this makefile
inc=h
src=src
out=out

# While this is the first target, the default target is
# specified in .DEFAULT_GOAL above.
all:
	@echo [$@]

$(out)/PmApiTest.exe: $(src)/PmApiTest.cpp $(src)/PmApiTest.def
	@echo [$@]
	gcc -Zomf -I$(inc) $< -o $@ $(src)/PmApiTest.def
	wrc -q -bt=os2 -zm $(src)/resource.rc $@

# The default target
default: $(out)/PmApiTest.exe
	@echo [$@]

build:
	@echo [$@]

clean:
	@echo [$@]
	rm -f -v $(out)/*.exe $(out)/*.res

dist:
	@echo [$@]
