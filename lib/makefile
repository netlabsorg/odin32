# $Id: makefile,v 1.6 2001-01-09 19:14:07 bird Exp $

#
# Odin32 API
#
#       support libraries makefile
#


#
# Include makefile.
#
!include ../makefile.inc


#
# All rule.
#
all:    $(ODIN32_LIB) \
        $(ODIN32_LIB)\pmwinx.lib \
        $(ODIN32_LIB)\libconv.lib \
        $(ODIN32_LIB)\libuls.lib \
        $(ODIN32_LIB)\wgss50.lib


#
# Make output director.
#
$(ODIN32_LIB):
    if not exist $(ODIN32_LIB) mkdir $(ODIN32_LIB)


#
# Make our own pmwinx (Open32) import library.
#
$(ODIN32_LIB)\pmwinx.lib: pmwinx.def
    $(IMPLIB) $(IMPLIBFLAGS) $(ODIN32_LIB)\pmwinx.lib pmwinx.def


#
# Make our own pmwinx (Open32) import library.
#
$(ODIN32_LIB)\wgss50.lib: wgss50.def
    $(IMPLIB) $(IMPLIBFLAGS) $(ODIN32_LIB)\wgss50.lib wgss50.def


#
# Make our own libconv (UNICODE) import library.
#
$(ODIN32_LIB)\libconv.lib: libconv.def
    $(IMPLIB) $(IMPLIBFLAGS) $(ODIN32_LIB)\libconv.lib libconv.def


#
# Make our own libuls (UNICODE) import library.
#
$(ODIN32_LIB)\libuls.lib: libuls.def
    $(IMPLIB) $(IMPLIBFLAGS) $(ODIN32_LIB)\libuls.lib libuls.def


#
# Make our own libuls import library.
#
clean:
    $(RM) $(ODIN32_LIB)\*.lib


#
# Dummy rules.
#
dep lib:

