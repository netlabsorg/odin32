# $Id: makefile,v 1.30 2001-09-30 00:32:39 bird Exp $

#
# Odin32 API
#
#       ole32.dll and regsvr32.exe makefile
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
    @$(DOMAKES) "ole32.mak regsvr32.mak" $(MAKE_CMD) $@


# don't have to make deps and libs more than once.
lib dep:
    @$(MAKE_CMD) -f ole32.mak $@


#
# Do not includes the common rules.
#

