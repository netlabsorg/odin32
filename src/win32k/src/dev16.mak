# $Id: dev16.mak,v 1.1.2.3 2002-04-07 22:40:37 bird Exp $

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
PATH_ROOT  = ..\..\..
!include $(PATH_ROOT)\make\setup.mak
!include ..\makefile.inc



#
# Config.
#
TARGET_MODE = SYSLIB
TARGET_NAME = Win32k_dev16
MAKEFILE    = Dev16.mak
CC_INCLUDES = -I../include -I../kKrnlLib/include -I$(PATH_DDKBASE)/h -I$(PATH_MSC)/include -I$(PATH_TOOLKIT)/h
ALL_DEFINES = -DWIN32K
CC_SEGS     = -NTCODE16 -NDDATA16


#
# Targets.
#
TARGET_OBJS =\
$(PATH_TARGET)\d16strat.$(EXT_OBJ)\
$(PATH_TARGET)\d16init.$(EXT_OBJ)\
$(PATH_TARGET)\d16globl.$(EXT_OBJ)\


#
# Process.
#
!include $(MAKE_INCLUDE_PROCESS)

