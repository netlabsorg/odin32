# $Id: makefile,v 1.5 2000-05-13 16:04:43 bird Exp $

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

PDWIN32_INCLUDE = ..\..\include
PDWIN32_TOOLS   = ..\bin
!include $(PDWIN32_INCLUDE)\pdwin32.mk


!ifdef DEBUG
MAKE_CMD = $(MAKE) -nologo DEBUG=1
!else
MAKE_CMD = $(MAKE) -nologo
!endif


all:    commonicc.lib commongcc.a commongcc.lib


needed: commonicc.lib


commonicc.lib: dummy
    -@echo $@
    @$(MAKE_CMD) -f makefile.icc

commongcc.a: dummy
    -@echo $@
    @$(MAKE_CMD) OMF=0 -f makefile.gcc

commongcc.lib: dummy
    -@echo $@
    @$(MAKE_CMD) OMF=1 -f makefile.gcc


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

