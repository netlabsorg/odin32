# $Id: MkCallTab.mak,v 1.2 2002-03-10 04:36:39 bird Exp $

#
# MkCallTab - 16-bit source generator.
#
#


#
# Setup.
#
!if [SET INCLUDE=]
!endif
BUILD_ENV  = MSCV6
PATH_ROOT  = ..\..\..\..
!include $(PATH_ROOT)\tools\make\setup.mak
!include ..\..\makefile.inc



#
# Config.
#
TARGET_MODE = EXE
TARGET_NAME = MkCallTab
MAKEFILE    = $(TARGET_NAME).mak
CC_INCLUDES = -I../include/kLib -I../include -I$(PATH_DDKBASE)/h -I$(PATH_MSC)/include -I$(PATH_TOOLKIT)/h
ALL_DEFINES = -DMKCALLTAB


#
# Targets.
#
TARGET_OBJS =\
$(PATH_TARGET)\MkCallTab.$(EXT_OBJ)\
$(PATH_TARGET)\krnlImportTable.$(EXT_OBJ)\
$(PATH_TARGET)\probkrnl.$(EXT_OBJ)\
$(PATH_TARGET)\d16globl.$(EXT_OBJ)\
$(PATH_TARGET)\vprntf16.$(EXT_OBJ)\

TARGET_LIBS =\
$(PATH_TOOLKIT)\lib\$(LIB_OS)\
$(PATH_MSC)\lib\$(LIB_C_OBJ)\


#
# Process.
#
!include $(MAKE_INCLUDE_PROCESS)

