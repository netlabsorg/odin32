# $Id: makefile,v 1.1 1999-06-01 08:43:08 phaller Exp $ 

#
# PD-Win32 API
#
#       support libraries makefile
#

PDWIN32_INCLUDE = ..\include
PDWIN32_BIN = ..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


all: pmwinx.lib


pmwinx.lib: pmwinx.def
	$(IMPLIB) pmwinx.lib pmwinx.def



clean:
        $(RM) *.LIB


