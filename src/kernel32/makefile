# $Id: makefile,v 1.116 2001-09-30 00:30:52 bird Exp $
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
    @$(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) $@
    @$(DOMAKES) "kernel32.mak" $(MAKE_CMD) $@


