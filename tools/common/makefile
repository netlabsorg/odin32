# $Id: makefile,v 1.9 2000-11-21 04:37:23 bird Exp $

#
# PD-Win32 API
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
!include ..\..\include\pdwin32.mk


#
# All rule.
#
all:    $(OBJDIR)\common.lib \
!ifdef GCC
        $(OBJDIR)\common.a \
!endif
        $(PDWIN32_TOOLS)\kDef2Wat.exe
#        $(PDWIN32_TOOLS)\kDump.exe


#
# The neede tools
#
needed:     $(OBJDIR)\common.lib \
            $(PDWIN32_TOOLS)\kDef2Wat.exe


#
# The library.
#
libs $(OBJDIR)\common.lib: force
    -@echo $@
    @$(MAKE_CMD) -f common.mak all

#
# The Aout library.
#
$(OBJDIR)\common.a: force
    -@echo $@
    @$(MAKE_CMD) OMF=1 -f common.mak all


#
# Tools.
#
$(PDWIN32_TOOLS)\kDump.exe    $(OBJDIR)\kDump.exe    kDump.exe \
$(PDWIN32_TOOLS)\kDef2Wat.exe $(OBJDIR)\kDef2Wat.exe kDef2Wat.exe : force
    -@echo $@
    @$(MAKE_CMD) -f $(@B).mak all


#
# Clean rule.
#
clean:
    $(MAKE_CMD) -f common.mak clean
    $(MAKE_CMD) -f kDef2Wat.mak clean
    $(MAKE_CMD) -f kDump.mak clean


#
# Dep rule - makes depenencies for C, C++ and Asm files.
#
dep:
    $(DEPEND) $(CINCLUDES) *.c *.cpp *.h *.asm *.inc \
        *.rc *.dlg $(PDWIN32_INCLUDE)\*.h -robj *.orc \
        -objo    *.c *.cpp \
        -objgobj *.c *.cpp


#
# A simple hack to make nmake process the target.
#
force:
     -@echo ...


#
# Lib rule - dummy rule
#
lib:

