# $Id: clib16.mak,v 1.4 2002-08-24 22:27:38 bird Exp $

#
# clib16
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
TARGET_NAME = clib16
TARGET_SUB  = kKrnlLib
MAKEFILE    = $(TARGET_NAME).mak

TARGET_OBJS =\
$(PATH_TARGET)\inp.$(EXT_OBJ)\
$(PATH_TARGET)\outp.$(EXT_OBJ)\
$(PATH_TARGET)\anfalmul.$(EXT_OBJ)\
$(PATH_TARGET)\anfaldiv.$(EXT_OBJ)\
$(PATH_TARGET)\anfalrem.$(EXT_OBJ)\
$(PATH_TARGET)\anfauldi.$(EXT_OBJ)\
$(PATH_TARGET)\anuldiv.$(EXT_OBJ)\
$(PATH_TARGET)\anldiv.$(EXT_OBJ)\
$(PATH_TARGET)\fmemcpy.$(EXT_OBJ)\
$(PATH_TARGET)\anulrem.$(EXT_OBJ)\
$(PATH_TARGET)\anlrem.$(EXT_OBJ)\
$(PATH_TARGET)\anlmul.$(EXT_OBJ)\
$(PATH_TARGET)\__AHINCR.$(EXT_OBJ)\
$(PATH_TARGET)\anlshl.$(EXT_OBJ)\

#
# Rules config.
#
!include $(MAKE_INCLUDE_PROCESS)

!if !$(BUILD_FORWARDING)

#
# Convert the library - temporary file.
#
$(PATH_TARGET)\_clib16_.lib: $(PATH_MSC)\lib\clibcep.lib
    $(PATH_TOOLS)\libconv.exe $** $@

#
# Extract required object files from the converted library.
#
$(TARGET_OBJS): $(PATH_TARGET)\_clib16_.lib
    $(AR) $(AR_FLAGS) $** *$@;

!endif
