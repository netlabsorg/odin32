# $Id: makefile,v 1.6 2000-09-28 03:08:07 bird Exp $
#
# Odin32 API
#
#       main glide makefile
#

# Directories
PDWIN32_INCLUDE = ..\..\..\include

# Common tools macros. (MAKE_CMD)
!include $(PDWIN32_INCLUDE)\pdwin32.tools

# Dummy all rule - nmake without target will make all targets of the first rule.
all_: all

# All, Lib, Clean and Dep rules.
dep clean lib all:
    cd cvg
    $(MAKE_CMD) $@
    cd ..\sst1
    $(MAKE_CMD) $@
    cd ..

