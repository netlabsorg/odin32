# $Id: Dev16.mak,v 1.7 2002-08-20 05:47:52 bird Exp $

#
# kKrnlLib/dev16
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
PATH_ROOT  = ..\..\..\..
!include $(PATH_ROOT)\make\setup.mak
!include ..\..\makefile.inc


#
# Config.
#
TARGET_MODE = SYSLIB
TARGET_NAME = kKrnlLib_dev16
MAKEFILE    = Dev16.mak
CC_INCLUDES = -I../include -I../kLib/include -I$(PATH_DDKBASE)/h -I$(PATH_MSC)/include -I$(PATH_TOOLKIT)/h
ALL_DEFINES = -DKKRNLLIB
CC_SEGS     = -NTCODE16 -NDDATA16


#
# Targets.
#
TARGET_OBJS =\
$(PATH_TARGET)\d16Strat.$(EXT_OBJ)\
$(PATH_TARGET)\d16Init.$(EXT_OBJ)\
$(PATH_TARGET)\d16Globl.$(EXT_OBJ)\
$(PATH_TARGET)\d16crt.$(EXT_OBJ)\
$(PATH_TARGET)\d16vprintf.$(EXT_OBJ)\
$(PATH_TARGET)\d16ProbeKrnl.$(EXT_OBJ)\
$(PATH_TARGET)\krnlImportTable.$(EXT_OBJ)\


#
# Process.
#
!include $(MAKE_INCLUDE_PROCESS)

