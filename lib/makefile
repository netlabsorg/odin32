# $Id: makefile,v 1.2 1999-06-07 13:35:59 sandervl Exp $ 

#
# PD-Win32 API
#
#       support libraries makefile
#

PDWIN32_INCLUDE = ..\include
PDWIN32_BIN = ..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


all: pmwinx.lib libconv.lib


pmwinx.lib: pmwinx.def
	$(IMPLIB) pmwinx.lib pmwinx.def

libconv.lib: libconv.def
	$(IMPLIB) libconv.lib libconv.def



clean:
        $(RM) *.LIB


