# $Id: makefile,v 1.7 2005/03/06 10:36:24 sao2l02 Exp $

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
    $(MAKE_CMD) CCENV=EMX -f libwrap0.mak $@
    $(MAKE_CMD) CCENV=EMX -f libwrap1.mak $@

# don't have to make deps more than once.
dep:
    $(MAKE_CMD) -f libwrap0.mak $@

#
# Dummy rules.
#
cleandlls:
cleanall:

#
# Do not includes the common rules.
#
