# $Id: makefile,v 1.27 2000-11-19 09:05:15 bird Exp $

#
# Odin32 API
#
#       ole32.dll and regsvr32.exe makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


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

