# $Id: Extract.mak,v 1.10 2002-08-24 22:07:40 bird Exp $

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
#!include ..\..\makefile.inc
ALL_INCLUDES    = -I../include -I../kLib/include
ALL_DEFINES     = -DEXTRACT -DLOGGING_DISABLED
PATH_ROOT       = ..\..\..\..
!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)
!include ..\..\makefile.inc

#
# Target config
#
TARGET_NAME     = Extract
TARGET_MODE     = EXE
TARGET_PUB_DIR  = $(PATH_TOOLS)
TARGET_STACKSIZE= 0x2000
MAKEFILE        = $(TARGET_NAME).mak
PREMAKEFILES_LIB= Extract16.mak

TARGET_OBJS =\
$(PATH_ROOT)\obj\$(SHT_TRGPLTFRM)$(SHT_BLDMD)mscv6-16\Extract16.$(EXT_LIB)\Extract16.$(EXT_LIB)\
$(PATH_TARGET)\Extract32.$(EXT_OBJ)\
$(PATH_TARGET)\krnlPrologs.$(EXT_OBJ)\

TARGET_LIBS =\
$(PATH_TOOLKIT)\lib\os2286.$(EXT_LIB)\
$(PATH_MSC)\lib\clibcep.$(EXT_LIB)\
$(LIB_OS)\
$(LIB_C_NRE)\

#
# Rules config
#
!include $(MAKE_INCLUDE_PROCESS)

