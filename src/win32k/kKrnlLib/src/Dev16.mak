# $Id: Dev16.mak,v 1.2 2002-03-10 04:26:17 bird Exp $

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
TARGET_MODE = SYSLIB
TARGET_NAME = kKrnlLib_dev16
CC_INCLUDES = -I../include/kLib -I../include -I$(PATH_DDKBASE)/h -I$(PATH_MSC)/include -I$(PATH_TOOLKIT)/h
ALL_DEFINES = -DKKRNLLIB
CC_SEGS     = -NTCODE16 -NDDATA16


#
# Targets.
#
TARGET_OBJS =\
$(PATH_TARGET)\d16Globl.$(EXT_OBJ)\
$(PATH_TARGET)\d16Init.$(EXT_OBJ)\
$(PATH_TARGET)\d16Strat.$(EXT_OBJ)\
$(PATH_TARGET)\ProbKrnl.$(EXT_OBJ)\
$(PATH_TARGET)\krnlImportTable.$(EXT_OBJ)\
$(PATH_TARGET)\vprntf16.$(EXT_OBJ)\


#
# Process.
#
!include $(MAKE_INCLUDE_PROCESS)

