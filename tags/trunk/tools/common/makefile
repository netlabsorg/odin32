# $Id: makefile,v 1.7 2000-10-03 05:42:39 bird Exp $

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
#            commonicc.lib: Build the IBM VA for C++ version of the library.
#            commongcc.a:   Build the gcc version of the library - aout
#            commongcc.lib: Build the gcc version of the library - OMF.
#
#

!include ..\..\include\pdwin32.mk


all:    commonicc.lib commongcc.a commongcc.lib


needed: commonicc.lib $(PDWIN32_TOOLS)\kDef2Wat.exe


commonicc.lib: dummy
    -@echo $@
    @$(MAKE_CMD) -f makefile.icc

commongcc.a: dummy
    -@echo $@
    @$(MAKE_CMD) OMF=0 -f makefile.gcc

commongcc.lib: dummy
    -@echo $@
    @$(MAKE_CMD) OMF=1 -f makefile.gcc


$(PDWIN32_TOOLS)\kDump.exe $(OBJDIR)\kDump.exe kDump.exe: dummy
    -@echo $@
    @$(MAKE_CMD) -f makefile.icc $@


$(PDWIN32_TOOLS)\kDef2Wat.exe $(OBJDIR)\kDef2Wat.exe kDef2Wat.exe: dummy
    -@echo $@
    @$(MAKE_CMD) -f makefile.icc $@


#a simple hack to make nmake process the target.
dummy:
     -@echo ...


clean:
    @$(MAKE_CMD) -f makefile.icc clean
    @$(MAKE_CMD) OMF=0 -f makefile.gcc clean
    @$(MAKE_CMD) OMF=1 -f makefile.gcc clean


dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h *.c *.cpp -objo *.c *.cpp -objgobj *.c *.cpp

