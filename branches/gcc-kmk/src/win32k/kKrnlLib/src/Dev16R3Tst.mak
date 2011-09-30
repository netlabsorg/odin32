# $Id: Dev16R3Tst.mak,v 1.8 2002-08-24 22:07:40 bird Exp $

#
# kKrnlLib/Dev16R3Tst
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
BUILD_ENV_FORCE = MSCV6-16
ALL_SEG_TEXT    = CODE16
ALL_SEG_DATA    = DATA16
CC_INCLUDES     = -I../include -I../kLib/include -I$(PATH_DDKBASE)/h -I$(PATH_MSC)/include -I$(PATH_TOOLKIT)/h
ALL_DEFINES     = -DKKRNLLIB -DR3TST
PATH_ROOT       = ..\..\..\..
!include $(PATH_ROOT)\make\setup.mak
!include ..\..\makefile.inc


#
# Target config.
#
TARGET_MODE     = SYSLIB
TARGET_NAME     = Dev16
TARGET_SUB      = kKrnlLib\R3Tst
MAKEFILE        = Dev16R3Tst.mak

TARGET_OBJS     =\
$(PATH_TARGET)\d16Globl.$(EXT_OBJ)\
$(PATH_TARGET)\d16Init.$(EXT_OBJ)\
$(PATH_TARGET)\d16Strat.$(EXT_OBJ)\
$(PATH_TARGET)\d16crt.$(EXT_OBJ)\
$(PATH_TARGET)\d16vprintf.$(EXT_OBJ)\
$(PATH_TARGET)\d16ProbeKrnl.$(EXT_OBJ)\
$(PATH_TARGET)\krnlImportTable.$(EXT_OBJ)\

#
# Rules config.
#
!include $(MAKE_INCLUDE_PROCESS)

