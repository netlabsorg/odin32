# $Id: makefile,v 1.12 2001-02-02 08:45:42 bird Exp $

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
$(ODIN32_TOOLS)\kDump.exe    $(OBJDIR)\kDump.exe    kDump.exe \
$(ODIN32_TOOLS)\kDef2Wat.exe $(OBJDIR)\kDef2Wat.exe kDef2Wat.exe : force
    -@echo $@
    @$(MAKE_CMD) -f $(@B).mak all


#
# Clean rule.
#
clean:  $(SYMBOLIC)
    $(MAKE_CMD) -f common.mak clean
    $(MAKE_CMD) -f kDef2Wat.mak clean
    $(MAKE_CMD) -f kDump.mak clean


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
     -@echo ...


#
# Lib rule - dummy rule
#
lib:    $(SYMBOLIC)

