# $Id: makefile,v 1.5 2000-03-04 19:59:20 bird Exp $
#
# PD-Win32 API
#
#       glu/nurbs makefile
#

PDWIN32_INCLUDE = ..\..\..\..\include
PDWIN32_LIB     = ..\..\..\..\lib
PDWIN32_BIN     = ..\..\..\..\bin
PDWIN32_TOOLS   = ..\..\..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


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

