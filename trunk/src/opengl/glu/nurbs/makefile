# $Id: makefile,v 1.3 2000-02-09 23:40:51 bird Exp $
#
# PD-Win32 API
#
#       glu/nurbs makefile
#

!ifdef DEBUG
!ifdef NODEBUGINFO
MAKE_CMD=nmake DEBUG=1 NODEBUGINFO=1
!else
MAKE_CMD=nmake DEBUG=1
!endif
!else
MAKE_CMD=nmake
!endif

all:
    cd interface
    $(MAKE_CMD)
    cd ..\internals
    $(MAKE_CMD)
    cd ..\nurbtess
    $(MAKE_CMD)
    cd ..


dep:
    cd interface
    $(MAKE_CMD) NODEP=1 dep
    cd ..\internals
    $(MAKE_CMD) NODEP=1 dep
    cd ..\nurbtess
    $(MAKE_CMD) NODEP=1 dep
    cd ..


clean:
    cd interface
    $(MAKE_CMD) clean
    cd ..\internals
    $(MAKE_CMD) clean
    cd ..\nurbtess
    $(MAKE_CMD) clean
    cd ..

