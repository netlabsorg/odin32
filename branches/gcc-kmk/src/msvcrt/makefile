# $Id: makefile,v 1.23 2003-01-10 13:13:49 sandervl Exp $

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

all clean cleanall lib:  # add all common rules here
!ifdef BUILD_MSVCRT    
    $(MAKE_CMD) -f msvcrt.mak $@
    $(MAKE_CMD) -f msvcrt20.mak $@
    $(MAKE_CMD) -f msvcrt40.mak $@
!endif
    $(MAKE_CMD) -f minivcrt.mak $@

# don't have to make deps more than once.
dep:
    $(MAKE_CMD) -f msvcrt.mak $@


#
# Do not includes the common rules.
#

