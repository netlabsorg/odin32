# $Id: makefile,v 1.24 2001-09-30 00:37:17 bird Exp $
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
    @$(DOMAKES) "winmm.mak" $(MAKE_CMD) $@
    @$(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) $@

