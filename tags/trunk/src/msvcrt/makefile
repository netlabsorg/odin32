# $Id: makefile,v 1.22 2003-01-06 13:26:34 sandervl Exp $

#
# Odin32 API
#
#       msvcrt.dll, msvcrt20.dll and msvcrt40.dll makefile
#

CCENV = EMX

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
    $(MAKE_CMD) -f msvcrt.mak $@
    $(MAKE_CMD) -f msvcrt20.mak $@
    $(MAKE_CMD) -f msvcrt40.mak $@

# don't have to make deps more than once.
dep:
    $(MAKE_CMD) -f msvcrt.mak $@


#
# Do not includes the common rules.
#

