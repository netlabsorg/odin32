# $Id: makefile,v 1.21 2001-09-30 00:34:23 bird Exp $

#
# Odin32 API
#
#       pe.exe and pec.exe makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Generic rules which forwards to the real makefiles.
#   The all rule has to stand alone, else make will make all the rules if it
#   is invoked without any target.
#
_all: all

$(COMMONRULES:dep lib=):
    @$(DOMAKES) "pe.mak pec.mak" $(MAKE_CMD) $@


# don't have to make deps more than once.
dep:
    @$(MAKE_CMD) -f pe.mak $@


# dummy rule.
lib:


#
# Do not includes the common rules.
#

