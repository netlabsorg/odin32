# $Id: makefile,v 1.20 2001-09-30 00:33:41 bird Exp $
#
# Odin32 API
#
#       opengl32.dll makefile
#


# Common tools macros. (MAKE_CMD)
!include ../../makefile.inc


# dummy all rule - invoking nmake withtout target caused it to do all of the targets below..
_all: all


# Subdirectories.
SUBDIRS = \
.\mesa \
.\glu \
.\glut \
.\glide \
.\mesa\3dfx

# All the common rules like all, lib, clean and dep.
$(COMMONRULES):
    @$(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) $@


