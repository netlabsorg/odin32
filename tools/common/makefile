# $Id: makefile,v 1.1 1999-09-05 02:09:18 bird Exp $

#
# Tools common parts
#
#       Top of the src tree makefile
#
#
#       Usage: nmake ( all | clean )
#
#            all: Build the entire src.
#            clean: Back to a "virgin" state.
#
#
#
!ifdef DEBUG
MAKE_CMD = nmake -nologo DEBUG=1
!else
MAKE_CMD = nmake -nologo
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



