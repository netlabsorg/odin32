# $Id: Extract16.mak,v 1.4 2002-04-07 22:39:44 bird Exp $

#
# Extract16 - 16-bit part of the kernel extract routine.
#
# Copyright (c) 1999-2002 knut st. osmundsen (bird@anduin.net)
#
# Project Odin Software License can be found in LICENSE.TXT
#


#
# Setup.
#
!if [SET INCLUDE=]
!endif
BUILD_ENV  = MSCV6
PATH_ROOT  = ..\..\..\..
!include $(PATH_ROOT)\make\setup.mak
!include ..\..\makefile.inc



#
# Config.
#
TARGET_MODE = LIB
TARGET_NAME = Extract16
MAKEFILE    = $(TARGET_NAME).mak
ALL_INCLUDES= -I../include -I$(PATH_DDKBASE)/h -I$(PATH_MSC)/include -I$(PATH_TOOLKIT)/h
ALL_DEFINES = -DEXTRACT


#
# Targets.
#
TARGET_OBJS =\
$(PATH_TARGET)\Extract.$(EXT_OBJ)\
$(PATH_TARGET)\d16Globl.$(EXT_OBJ)\
$(PATH_TARGET)\d16crt.$(EXT_OBJ)\
$(PATH_TARGET)\d16vprintf.$(EXT_OBJ)\
$(PATH_TARGET)\d16ProbeKrnl.$(EXT_OBJ)\
$(PATH_TARGET)\krnlImportTable.$(EXT_OBJ)\


#
# Process.
#
!include $(MAKE_INCLUDE_PROCESS)
