# $Id: Extract.mak,v 1.4 2002-03-31 19:01:05 bird Exp $

#
# kKrnlLib/krnl makefile.
#
#

#
# Load the build setup.
#
!include ..\..\makefile.inc
PATH_ROOT = ..\..\..\..
!include $(PATH_ROOT)\tools\make\setup.mak
PATH_BIN   = $(PATH_TOOLS)


#
# Config.
#
TARGET_NAME = Extract
MAKEFILE    = $(TARGET_NAME).mak
ALL_INCLUDES= -I../include
ALL_DEFINES = -DEXTRACT -DLOGGING_DISABLED
TARGET_STACKSIZE = 0x2000


#
# Object files.
#
PREMAKEFILES = Extract16.mak

TARGET_OBJS =\
$(PATH_ROOT)\obj\$(NAME_1)$(NAME_2)mscv6\Extract16.$(EXT_LIB)\Extract.$(EXT_OBJ)\
$(PATH_TARGET)\Extract32.$(EXT_OBJ)\
$(PATH_TARGET)\krnlPrologs.$(EXT_OBJ)\

TARGET_LIBS =\
$(PATH_ROOT)\obj\$(NAME_1)$(NAME_2)mscv6\Extract16.$(EXT_LIB)\Extract16.$(EXT_LIB)\
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

