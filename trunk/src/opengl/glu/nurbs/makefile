# $Id: makefile,v 1.1 2000-02-09 08:50:55 jeroen Exp $
#
# PD-Win32 API
#
#       glu/nurbs makefile
#

!ifdef DEBUG
MAKE_CMD=nmake DEBUG=1
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

clean:
        cd interface
        $(MAKE_CMD) clean
        cd ..\internals
        $(MAKE_CMD) clean
        cd ..\nurbtess
        $(MAKE_CMD) clean
        cd ..
