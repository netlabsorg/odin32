# $Id: makefile,v 1.17 2001-09-30 00:39:51 bird Exp $

#
# Odin32
#
#   Top of the tree makefile
#
#
#   Usage: nmake ( debug | nodebuginfo | profile | release | all | dep | clean )
#
#            debug: Change to a debug build.
#            debugsmp: Start nmake process in background that processes all
#                      dlls in reverse
#            nodebuginfo: Change to a debug build without debug info in binaries
#            nodebuginfosmp: Change to an SMP debug build without debug info in binaries
#            profile: Change to a debug build with profiling hooks
#            release: Change to a release build.
#            releasesmp: Change to an SMP release build.
#            all: Build the entire tree.
#            dep: Make dependencies for the entire tree.
#            clean: Bring tree back to a "virgin" state.
#
#
#


#
# Check that makefile.inc exists, If it doesn't we'll try make it.
#
!if [.\tools\bin\Exists.cmd makefile.inc] == 1
!   if [configure.cmd noninteractive]
!   endif
!endif


#
# Include makefile.
#
!include makefile.inc


#
# (Re?)Define make command.
#
MAKE_CMD = $(MAKE) -nologo


#
# Build which are normally built.
#
BLDDIRS = src tools\install
ALL_DIRS = include lib src tools


#
# The ordinary rules.
#
all:            odin_libraries  needed
    @$(DODIRS) "$(BLDDIRS)"     $(MAKE_CMD) all

clean nothing:
    @$(DODIRS) "$(ALL_DIRS)"    $(MAKE_CMD) $@

libs: lib
lib dep:        needed
    @$(DODIRS) "$(ALL_DIRS)"    $(MAKE_CMD) $@


#
# Special build mode rules.
#
debug:          odin_libraries  needed
    @$(DODIRS) "$(BLDDIRS)"     $(MAKE_CMD) DEBUG=1 all

debugsmp:       odin_libraries  needed
    @$(DODIRS) "src"            $(MAKE_CMD) DEBUG=1 smp
    @$(DODIRS) "tools\install"  $(MAKE_CMD) DEBUG=1 all

nodebuginfo:    odin_libraries  needed
    @$(DODIRS) "$(BLDDIRS)"     $(MAKE_CMD) DEBUG=1 NODEBUGINFO=1 all

nodebuginfosmp: odin_libraries  needed
    @$(DODIRS) "src"            $(MAKE_CMD) DEBUG=1 NODEBUGINFO=1 smp
    @$(DODIRS) "tools\install"  $(MAKE_CMD) DEBUG=1 NODEBUGINFO=1 all

profile:        odin_libraries  needed
    @$(DODIRS) "$(BLDDIRS)"     $(MAKE_CMD) DEBUG=1 PROFILE=1 all

profilesmp:     odin_libraries  needed
    @$(DODIRS) "src"            $(MAKE_CMD) DEBUG=1 PROFILE=1 smp
    @$(DODIRS) "tools\install"  $(MAKE_CMD) DEBUG=1 PROFILE=1 all

release:        odin_libraries  needed
    SET DEBUG=
    @$(DODIRS) "$(BLDDIRS)"     $(MAKE_CMD) all

releasesmp:     odin_libraries  needed
    SET DEBUG=
    @$(DODIRS) "src"            $(MAKE_CMD) smp
    @$(DODIRS) "tools\install"  $(MAKE_CMD) all


#
# Common rules.
#
odin_libraries:
    @$(DODIRS) "lib"            $(MAKE_CMD)

needed_tools: needed
needed:
    @$(DODIRS) "tools"          $(MAKE_CMD) $@

