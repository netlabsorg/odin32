# $Id: makefile,v 1.1 2001-04-26 19:26:11 sandervl Exp $

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

all clean:  # add all common rules here
    $(MAKE_CMD) -f ole32.mak $@
    $(MAKE_CMD) -f regsvr32.mak $@


# don't have to make deps and libs more than once.
lib dep:
    $(MAKE_CMD) -f ole32.mak $@


#
# Do not includes the common rules.
#

