###############################################################################
# GNUmakefile :: Build the ApiTestPm Program                                  #
# --------------------------------------------------------------------------- #
# For this initial version we start with a PM version because, well, we are   #
# on OS/2! However, this is not the module of focus since it is not directly  #
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

# While this is the first target, the true default target is
# specified in .DEFAULT_GOAL above. Here we'll redirect to target 'default'.
all: default
#	@echo [$@]

# PM version of ApiTest
$(out)/ApiTestPm.exe: $(src)/ApiTestPm.cpp $(out)/ApiTestPm.res $(src)/ApiTestPm.def
	@echo [$@]
	gcc -Zomf -I$(inc) $< -o $@ $(src)/ApiTestPm.def
	wrc -q -bt=os2 -zm $*.res $@

$(out)/ApiTestPm.res: $(src)/ApiTestPm.rc
	@echo [$@]
	wrc -q -r -bt=os2 -zm -fo=$@ $<

# The default target
default: $(out)/ApiTestPm.exe
#	@echo [$@]

# Redirect to target 'default'
build: default
#	@echo [$@]

# Cleanup files
clean:
	@echo [$@]
	rm -f -v $(out)/*.exe $(out)/*.res

# Run the program
run: build
	@echo [$@]
	$(out)/ApiTestPm.exe

# Distribute to somewhere (NI)
dist:
	@echo [$@]
