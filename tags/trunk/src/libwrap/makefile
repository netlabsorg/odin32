# $Id: makefile,v 1.6 2004-12-04 10:22:28 sao2l02 Exp $

#
# Odin32 API
#
#       libwrap0.lib + libwrap1.lib makefile
#       splitted for nmake, makros need smaller as 64k

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

all clean lib:  # add all common rules here
    $(MAKE_CMD) -f libwrap0.mak $@
    $(MAKE_CMD) -f libwrap1.mak $@

# don't have to make deps more than once.
dep:
    $(MAKE_CMD) -f libwrap0.mak $@


#
# Do not includes the common rules.
#

