# $Id: Extract.mak,v 1.9 2002-08-22 02:55:48 bird Exp $

#
# kKrnlLib/krnl makefile.
#
# Copyright (c) 1999-2002 knut st. osmundsen (bird@anduin.net)
#
# GPL
#

#
# Setup config
#
!include ..\..\makefile.inc
PATH_ROOT = ..\..\..\..
!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)

#
# Target config
#
TARGET_NAME     = Extract
TARGET_PUBNAME  = $(PATH_TOOLS)\$(TARGET_NAME).$(TARGET_EXT)
TARGET_STACKSIZE= 0x2000
ALL_INCLUDES    = -I../include -I../kLib/include
ALL_DEFINES     = -DEXTRACT -DLOGGING_DISABLED
MAKEFILE        = $(TARGET_NAME).mak
PREMAKEFILES    = Extract16.mak

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
# Rules config
#
!include $(MAKE_INCLUDE_PROCESS)

