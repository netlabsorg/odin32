# $Id: Makefile,v 1.2 2002-10-14 15:17:05 bird Exp $

#
# kKrnlLib.
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#


#
# Setup config.
#
AS_INCLUDES     = -I$(PATH_DDKBASE)\inc
CC_INCLUDES     = -I$(PATH_TOOLKIT)\h -I$(PATH_DDKBASE)\h
ALL_DEFINES     =
BUILD_ENV_FORCE = VAC308
PATH_ROOT       = ..\..\..\..\..
!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)

#
# Walk config.
#
#PREMAKEFILES_LIB= clib16.mak devhelp16.mak


#
# Target config.
#
TARGET_MODE     = EXE
TARGET_NAME     = kDevTest
TARGET_SUB      = Tools

TARGET_OBJS     = \
$(PATH_TARGET)\kDevTest.$(EXT_OBJ)\
$(PATH_TARGET)\kdtDevHelp.$(EXT_OBJ)\
$(PATH_TARGET)\kdtLoad.$(EXT_OBJ)\
$(PATH_TARGET)\kdtInit.$(EXT_OBJ)\
$(PATH_TARGET)\kdtStrategy.$(EXT_OBJ)\

TARGET_LIBS     = \
$(LIB_OS)\
$(LIB_C_OBJ)\

#
# Rule config.
#
RULES_FORWARD = $(TARGET).$(EXT_ILIB) $(TARGET_IDEF) $(TARGET_ILIB)
!include $(MAKE_INCLUDE_PROCESS)

