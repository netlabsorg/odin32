# $Id: Extract.mak,v 1.8 2002-08-20 05:47:53 bird Exp $

#
# kKrnlLib/krnl makefile.
#
# Copyright (c) 1999-2002 knut st. osmundsen (bird@anduin.net)
#
# GPL
#

#
# Load the build setup.
#
!include ..\..\makefile.inc
PATH_ROOT = ..\..\..\..
!include $(PATH_ROOT)\make\setup.mak
PATH_BIN   = $(PATH_TOOLS)


#
# Config.
#
TARGET_NAME = Extract
MAKEFILE    = $(TARGET_NAME).mak
ALL_INCLUDES= -I../include -I../kLib/include
ALL_DEFINES = -DEXTRACT -DLOGGING_DISABLED
TARGET_STACKSIZE = 0x2000


#
# Object files.
#
PREMAKEFILES = Extract16.mak

TARGET_OBJS =\
$(PATH_ROOT)\obj\$(SHT_TRGPLTFRM)$(SHT_BLDMD)mscv6-16\Extract16.$(EXT_LIB)\Extract.$(EXT_OBJ)\
$(PATH_TARGET)\Extract32.$(EXT_OBJ)\
$(PATH_TARGET)\krnlPrologs.$(EXT_OBJ)\

TARGET_LIBS =\
$(PATH_ROOT)\obj\$(SHT_TRGPLTFRM)$(SHT_BLDMD)mscv6-16\Extract16.$(EXT_LIB)\Extract16.$(EXT_LIB)\
$(PATH_TOOLKIT)\lib\os2286.lib\
$(PATH_MSC)\lib\clibcep.lib\
!if "$(BUILD_ENV)" == "VAC308"
cppon30.$(EXT_LIB)\
!else
!if "$(BUILD_ENV)" == "VAC365"
cpprni36.$(EXT_LIB)\
!else
$(LIB_C_OBJ)\
!endif
!endif
$(LIB_OS)\


#
# Load the build process rules.
#
!include $(MAKE_INCLUDE_PROCESS)
