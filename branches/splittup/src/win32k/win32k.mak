# $Id: win32k.mak,v 1.13.2.5 2002-04-07 22:40:36 bird Exp $

#
# win32k.sys
#
# Copyright (c) 1998-2002 knut st. osmundsen (bird@anduin.net)
#


#
# Setup.
#
PATH_ROOT = ..\..
!include $(PATH_ROOT)\make\setup.mak


#
# kLib overrides variables.
#
!if [SET KLIB_INC=$(MAKEDIR)\kLib.inc]
!endif


#
# Config.
#
TARGET_MODE = SYS
TARGET_NAME = win32k
MAKEFILE    = $(TARGET_NAME).mak


#
# Object files.
#
TARGET_OBJS =\
$(PATH_OBJ)\Win32k_first.$(EXT_LIB)\Win32k_first.$(EXT_LIB)\
$(PATH_OBJ)\Win32k_src.$(EXT_LIB)\Win32k_src.$(EXT_LIB)\

TARGET_LIBS =\
kKrnlLib\kKrnlLib.$(EXT_LIB)\
os2386p.$(EXT_LIB)\
os2386.$(EXT_LIB)\
os2286p.$(EXT_LIB)\
!if "$(BUILD_ENV)" == "VAC308"
cppon30.$(EXT_LIB)\
!else
!if "$(BUILD_ENV)" == "VAC365"
cpprni36.$(EXT_LIB)\
!else
$(LIB_C_OBJ)\
!endif
!endif
$(PATH_OBJ)\Win32k_last.$(EXT_LIB)\Win32k_last.$(EXT_LIB)

TARGET_DEPS =\
$(PATH_OBJ)\Win32k_last.$(EXT_LIB)\Win32k_last.$(EXT_LIB)

#SUBDIRS     = kKrnlLib include src
SUBDIRS     = include src


#
# Process
#
!include $(MAKE_INCLUDE_PROCESS)

