# $Id: makefile,v 1.17 2000-03-06 23:33:42 bird Exp $
#
# Odin32 API
#
#       opengl32.dll makefile
#

# Directories
PDWIN32_INCLUDE = ..\..\include

# Common tools macros. (MAKE_CMD)
!include $(PDWIN32_INCLUDE)\pdwin32.mk


# dummy all rule - invoking nmake withtout target caused it to do all of the targets below..
_all: all


# All, Lib, Clean and Dep rules.
clean dep all lib:
    cd mesa
    $(MAKE_CMD) $@
    cd ..\glu
    $(MAKE_CMD) $@
    cd ..\glut
    $(MAKE_CMD) $@
    cd ..\glide
    $(MAKE_CMD) $@
    cd ..\mesa\3dfx
    $(MAKE_CMD) $@
    cd ..\..

