# $Id: Dev16R3Tst.mak,v 1.7 2002-08-22 03:19:27 bird Exp $

#
# kKrnlLib/Dev16R3Tst
#
# Copyright (c) 1998-2002 knut st. osmundsen (bird@anduin.net)
#
# GPL
#


#
# Setup.
#
!if [SET INCLUDE=]
!endif
BUILD_ENV_FORCE = MSCV6-16
ALL_SEG_TEXT    = CODE16
ALL_SEG_DATA    = DATA16
PATH_ROOT       = ..\..\..\..
!include $(PATH_ROOT)\make\setup.mak
!include ..\..\makefile.inc


#
# Config.
#
TARGET_MODE = SYSLIB
TARGET_NAME = kKrnlLib_Dev16R3Tst
MAKEFILE    = Dev16R3Tst.mak
CC_INCLUDES = -I../include -I../kLib/include -I$(PATH_DDKBASE)/h -I$(PATH_MSC)/include -I$(PATH_TOOLKIT)/h
ALL_DEFINES = -DKKRNLLIB -DR3TST


#
# Targets.
#
TARGET_OBJS =\
$(PATH_TARGET)\d16Globl.$(EXT_OBJ)\
$(PATH_TARGET)\d16Init.$(EXT_OBJ)\
$(PATH_TARGET)\d16Strat.$(EXT_OBJ)\
$(PATH_TARGET)\d16crt.$(EXT_OBJ)\
$(PATH_TARGET)\d16vprintf.$(EXT_OBJ)\
$(PATH_TARGET)\d16ProbeKrnl.$(EXT_OBJ)\
$(PATH_TARGET)\krnlImportTable.$(EXT_OBJ)\


#
# Process.
#
!include $(MAKE_INCLUDE_PROCESS)

