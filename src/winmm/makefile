# $Id: makefile,v 1.22 2000-12-16 22:47:28 bird Exp $
#
# Odin32 API
#
#       winmm.dll and releated dlls makefile
#


# Common tools macros. (MAKE_CMD)
!include ../../makefile.inc


# dummy all rule - invoking nmake withtout target caused it to do all of the targets below..
_all: all


# Subdirectories.
SUBDIRS = \
mcicda


# All the common rules like all, lib, clean and dep.
$(COMMONRULES):
    $(MAKE_CMD) -f winmm.mak $@
    $(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) $@


