# $Id: Dev16.mak,v 1.10 2002-12-16 01:52:23 bird Exp $

#
# kKrnlLib/dev16
#
# Copyright (c) 1998-2002 knut st. osmundsen (bird@anduin.net)
#
# GPL
#

#
# Setup config.
#
!if [SET INCLUDE=]
!endif
PATH_ROOT       = ..\..\..\..
BUILD_ENV_FORCE = MSCV6-16
ALL_SEG_TEXT    = CODE16
ALL_SEG_DATA    = DATA16
ALL_DEFINES     = -DKKRNLLIB
CC_INCLUDES     = -I../include -I../kLib/include -I$(PATH_DDKBASE)/h -I$(PATH_MSC)/include -I$(PATH_TOOLKIT)/h
!include $(PATH_ROOT)\make\setup.mak
#!include ..\..\makefile.inc

#
# Target config
#
TARGET_MODE     = SYSLIB
TARGET_NAME     = dev16
TARGET_SUB      = kKrnlLib
MAKEFILE        = $(TARGET_NAME).mak

TARGET_OBJS     =\
$(PATH_TARGET)\d16Strat.$(EXT_OBJ)\
$(PATH_TARGET)\d16Init.$(EXT_OBJ)\
$(PATH_TARGET)\d16Globl.$(EXT_OBJ)\
$(PATH_TARGET)\devErrors.$(EXT_OBJ)\
# $(PATH_TARGET)\d16crt.$(EXT_OBJ)\
$(PATH_TARGET)\d16vprintf.$(EXT_OBJ)\
# $(PATH_TARGET)\d16ProbeKrnl.$(EXT_OBJ)\
$(PATH_TARGET)\krnlImportTable.$(EXT_OBJ)\

#
# Rules config.
#
!include $(MAKE_INCLUDE_PROCESS)

