# $Id: Extract.mak,v 1.11 2002-12-16 02:24:27 bird Exp $

#
# kKrnlLib/krnl makefile.
#
# Copyright (c) 1999-2003 knut st. osmundsen (bird@anduin.net)
#
# GPL
#

#
# Setup config
#
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
MAKEFILE        = $(TARGET_NAME).mak

TARGET_OBJS =\
$(PATH_TARGET)\Extract32.$(EXT_OBJ)\
$(PATH_TARGET)\krnlPrologs.$(EXT_OBJ)\
$(PATH_TARGET)\krnlImportTable.$(EXT_OBJ)\

TARGET_LIBS =\
$(LIB_OS)\
$(LIB_C_NRE)\

#
# Rules config
#
!include $(MAKE_INCLUDE_PROCESS)

