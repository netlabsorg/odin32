# $Id: makefile,v 1.3 2005-05-08 16:30:36 abwillis Exp $
#
# Odin32 API, common wine libs
#
#       libs makefile
#


# Common tools macros. (MAKE_CMD)
!include ../makefile.inc


# dummy all rule - invoking nmake withtout target caused it to do all of the targets below..
_all needed: all


# Subdirectories.
SUBDIRS = \
unicode \
wpp \
port \


# All the common rules like all, lib, clean and dep.
$(COMMONRULES):
    @$(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) $@

