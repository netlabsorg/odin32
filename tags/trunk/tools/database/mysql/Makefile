# $Id: Makefile,v 1.2 2002-02-12 07:05:01 bird Exp $

#
# Odin32 API
#
#       support libraries makefile
#


#
# Include makefile.
#
!include ../../../makefile.inc


#
# All rule.
#
all:    $(ODIN32_LIB) \
        $(ODIN32_LIB)\mysql.lib \


#
# Make output director.
#
$(ODIN32_LIB):
    @if not exist $(ODIN32_LIB) mkdir $(ODIN32_LIB)


#
# Make our own mysql import library.
#
$(ODIN32_LIB)\mysql.lib: mysql.def
    $(IMPLIB) $(IMPLIBFLAGS) $(ODIN32_LIB)\mysql.lib mysql.def


#
# Make our own libuls import library.
#
clean:
    $(RM) $(ODIN32_LIB)\mysql.lib


#
# Dummy rules.
#
dep lib:


#
# Nothing rule used for makefile debugging.
#
nothing:
    @echo Did nothing with makefile $(MAKEDIR)\$(MAKEFILE)

