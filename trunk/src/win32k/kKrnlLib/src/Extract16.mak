# $Id: Extract16.mak,v 1.1 2002-03-10 04:28:03 bird Exp $

#
# kKrnlLib/dev16
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
TARGET_MODE = LIB
TARGET_NAME = Extract16
MAKEFILE    = $(TARGET_NAME).mak
CC_INCLUDES = -I../include/kLib -I../include -I$(PATH_DDKBASE)/h -I$(PATH_MSC)/include -I$(PATH_TOOLKIT)/h
ALL_DEFINES = -DEXTRACT


#
# Targets.
#
TARGET_OBJS =\
$(PATH_TARGET)\Extract.$(EXT_OBJ)\
$(PATH_TARGET)\d16Globl.$(EXT_OBJ)\
$(PATH_TARGET)\ProbKrnl.$(EXT_OBJ)\
$(PATH_TARGET)\krnlImportTable.$(EXT_OBJ)\
$(PATH_TARGET)\vprntf16.$(EXT_OBJ)\


#
# Process.
#
!include $(MAKE_INCLUDE_PROCESS)

