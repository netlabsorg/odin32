# $Id: makefile,v 1.4 1999-06-09 13:07:41 sandervl Exp $ 

#
# PD-Win32 API
#
#       support libraries makefile
#

PDWIN32_INCLUDE = ..\include
PDWIN32_BIN = ..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


all: pmwinx.lib libconv.lib libuls.lib


pmwinx.lib: pmwinx.def
	$(IMPLIB) pmwinx.lib pmwinx.def

libconv.lib: libconv.def
	$(IMPLIB) libconv.lib libconv.def

libuls.lib: libuls.def
	$(IMPLIB) libuls.lib libuls.def


clean:
        $(RM) *.LIB


