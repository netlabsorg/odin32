# $Id: makefile,v 1.19 2000-12-02 23:57:55 bird Exp $

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

all clean:  # add all common rules here
    $(MAKE_CMD) -f pe.mak $@
    $(MAKE_CMD) -f pec.mak $@


# don't have to make deps more than once.
dep:
    $(MAKE_CMD) -f pe.mak $@


# dummy rule.
lib:


#
# Do not includes the common rules.
#

