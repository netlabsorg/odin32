# $Id: makefile,v 1.18 2000-11-19 08:58:10 bird Exp $
#
# Odin32 API
#
#       opengl32.dll makefile
#


# Common tools macros. (MAKE_CMD)
!include ../../include/pdwin32.mk


# dummy all rule - invoking nmake withtout target caused it to do all of the targets below..
_all: all


# Subdirectories.
SUBDIRS = \
.\mesa \
.\glu \
.\glut \
.\glide \
.\mesa\3dfx

# All, Lib, Clean and Dep rules.
clean dep all lib:
    $(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) $@

