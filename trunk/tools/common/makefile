# $Id: makefile,v 1.15 2001-10-01 01:35:38 bird Exp $

#
# Odin32 API
#
#       Tools common parts
#
#       Usage: nmake ( all | clean | dep |..)
#
#            all: Build the entire src.
#            clean: Back to a "virgin" state.
#            dep: Creates the .depend file.
#            needed: Builds the tools used during compilation of the src tree.
#            common.lib or libs: Make common.lib.
#            kDef2Wat.exe: kDef2Wat.exe
#            kDump.exe: kDump.exe
#
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# All rule.
#
all:    $(OBJDIR)\common.lib \
!ifdef GCC
        $(OBJDIR)\common.a \
!endif
        $(ODIN32_TOOLS)\kDef2Wat.exe \
        $(ODIN32_TOOLS)\kDump.exe


#
# The neede tools
#
needed:     $(OBJDIR)\common.lib \
            $(ODIN32_TOOLS)\kDef2Wat.exe \
            $(SYMBOLIC)

#
# The library.
#
libs $(OBJDIR)\common.lib: force
    @$(DOMAKES) "common.mak" $(MAKE_CMD) all

#
# The Aout library.
#
$(OBJDIR)\common.a: force
    @$(DOMAKES) "common.mak" $(MAKE_CMD) OMF=1 all


#
# Tools.
#
$(ODIN32_TOOLS)\kDump.exe    $(OBJDIR)\kDump.exe    kDump.exe \
$(ODIN32_TOOLS)\kDef2Wat.exe $(OBJDIR)\kDef2Wat.exe kDef2Wat.exe : force
    @$(DOMAKES) "$(@B).mak" $(MAKE_CMD) all


#
# Clean rule.
#
clean:  $(SYMBOLIC)
    @$(DOMAKES) "common.mak kDef2Wat.mak kDump.mak" $(MAKE_CMD) clean


#
# Dep rule - makes depenencies for C, C++ and Asm files.
#
dep:    $(SYMBOLIC)
    $(DEPEND) $(CINCLUDES) *.c *.cpp *.h *.asm *.inc \
        *.rc *.dlg $(ODIN32_INCLUDE)\*.h -robj *.orc \
        -objo    *.c *.cpp \
        -objgobj *.c *.cpp


#
# A simple hack to make nmake process the target.
#
force:  $(SYMBOLIC)
#!ifdef MAKEVER
     -@echo ...
#!endif


#
# Lib rule - dummy rule
#
lib:    $(SYMBOLIC)


#
# Nothing rule used for makefile debugging.
#
nothing:
    @echo Did nothing with makefile $(MAKEDIR)\$(MAKEFILE)


