# $Id: devhelp16.mak,v 1.2 2002-04-07 22:40:15 bird Exp $

#
# devhelp (16-bit)
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#


#
# Setup.
#
PATH_ROOT = ..\..\..
!include $(PATH_ROOT)\make\setup.mak
!include ..\makefile.inc


#
# Config.
#
TARGET_MODE = LIB
TARGET_NAME = kKrnlLib_devhelp
MAKEFILE = devhelp16.mak


#
# Object files.
#
TARGET_OBJS =\
$(PATH_TARGET)\dhcall5b.$(EXT_OBJ)\
$(PATH_TARGET)\dhcal11a.$(EXT_OBJ)\
$(PATH_TARGET)\dhcal11e.$(EXT_OBJ)\
$(PATH_TARGET)\dhcal11g.$(EXT_OBJ)\
$(PATH_TARGET)\dhret.$(EXT_OBJ)\
$(PATH_TARGET)\dhcall2e.$(EXT_OBJ)\


#
# Process
#
!include $(MAKE_INCLUDE_PROCESS)


#
# Convert devhelp library - temporary file.
#
$(PATH_TARGET)\_devhelp_.lib: $(PATH_DDKBASE)\lib\dhcalls.lib
    $(PATH_TOOLS)\libconv.exe $** $@

#
# Extract required object files from the converted devhelp library.
#
$(TARGET_OBJS): $(PATH_TARGET)\_devhelp_.lib
    $(AR) $(AR_FLAGS) $** *$@;

