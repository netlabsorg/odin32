# $Id: makefile,v 1.4 2000-03-04 19:49:03 bird Exp $
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
    $(MAKE_CMD) dep
    cd ..\internals
    $(MAKE_CMD) dep
    cd ..\nurbtess
    $(MAKE_CMD) dep
    cd ..


clean:
    cd interface
    $(MAKE_CMD) clean
    cd ..\internals
    $(MAKE_CMD) clean
    cd ..\nurbtess
    $(MAKE_CMD) clean
    cd ..

