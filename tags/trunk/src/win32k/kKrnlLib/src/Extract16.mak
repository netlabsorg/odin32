# $Id: Extract16.mak,v 1.9 2002-08-24 22:07:40 bird Exp $

#
# Extract16 - 16-bit part of the kernel extract routine.
#
# Copyright (c) 1999-2002 knut st. osmundsen (bird@anduin.net)
#
# GPL
#
#

#
# Setup config
#
!if [SET INCLUDE=]
!endif
BUILD_ENV_FORCE = MSCV6-16
ALL_INCLUDES    = -I../include -I../kLib/include -I$(PATH_DDKBASE)/h -I$(PATH_MSC)/include -I$(PATH_TOOLKIT)/h
ALL_DEFINES     = -DEXTRACT
PATH_ROOT       = ..\..\..\..
!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)

#
# Target config
#
TARGET_MODE     = LIB
TARGET_NAME     = Extract16
MAKEFILE        = $(TARGET_NAME).mak

TARGET_OBJS     =\
$(PATH_TARGET)\Extract.$(EXT_OBJ)\
$(PATH_TARGET)\d16Globl.$(EXT_OBJ)\
$(PATH_TARGET)\d16crt.$(EXT_OBJ)\
$(PATH_TARGET)\d16vprintf.$(EXT_OBJ)\
$(PATH_TARGET)\d16ProbeKrnl.$(EXT_OBJ)\
$(PATH_TARGET)\krnlImportTable.$(EXT_OBJ)\

#
# Rules config
#
!include $(MAKE_INCLUDE_PROCESS)

