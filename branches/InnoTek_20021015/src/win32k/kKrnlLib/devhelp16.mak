# $Id: devhelp16.mak,v 1.3 2002-08-24 22:27:38 bird Exp $

#
# devhelp (16-bit)
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#

#
# Setup config.
#
PATH_ROOT = ..\..\..
!include $(PATH_ROOT)\make\setup.mak
!include ..\makefile.inc

#
# Target config.
#
TARGET_MODE = LIB
TARGET_NAME = devhelp16
TARGET_SUB  = kKrnlLib
MAKEFILE    = $(TARGET_NAME).mak

TARGET_OBJS =\
$(PATH_TARGET)\dhcall5b.$(EXT_OBJ)\
$(PATH_TARGET)\dhcal11a.$(EXT_OBJ)\
$(PATH_TARGET)\dhcal11e.$(EXT_OBJ)\
$(PATH_TARGET)\dhcal11g.$(EXT_OBJ)\
$(PATH_TARGET)\dhret.$(EXT_OBJ)\
$(PATH_TARGET)\dhcall2e.$(EXT_OBJ)\

#
# Rules config.
#
!include $(MAKE_INCLUDE_PROCESS)

!if !$(BUILD_FORWARDING)

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

!endif

