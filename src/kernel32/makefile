# $Id: makefile,v 1.114 2001-04-03 14:10:47 sandervl Exp $
#
# Odin32 API
#
#       kernel32 makefile
#


# Common tools macros. (MAKE_CMD)
!include ../../makefile.inc


# dummy all rule - invoking nmake withtout target caused it to do all of the targets below..
_all: all


# Subdirectories.
SUBDIRS = \
unicode


# All the common rules like all, lib, clean and dep.
$(COMMONRULES):
    $(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) $@
    $(MAKE_CMD) -f kernel32.mak $@


