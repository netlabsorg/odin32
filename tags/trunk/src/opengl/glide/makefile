# $Id: makefile,v 1.4 2000-03-06 23:33:43 bird Exp $
#
# Odin32 API
#
#       main glide makefile
#

# Directories
PDWIN32_INCLUDE = ..\..\include

# Common tools macros. (MAKE_CMD)
!include ..\..\..\include\pdwin32.tools


# All, Lib, Clean and Dep rules.
all lib clean dep:
    cd cvg
    $(MAKE_CMD) $@
    cd ..\sst1
    $(MAKE_CMD) $@
    cd ..

