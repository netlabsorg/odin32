# $Id: kKrnlLib.mak,v 1.6 2002-08-22 03:07:50 bird Exp $

#
# kKrnlLib.
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#


#
# Setup config.
#
PATH_ROOT = ..\..\..
!include $(PATH_ROOT)\make\setup.mak

#
# kLib overrides variables.
#
!if [SET KLIB_INC=$(MAKEDIR)\kLib.inc]
!endif

#
# Target config.
#
TARGET_MODE     = SYS
TARGET_NAME     = kKrnlLib
TARGET_ILIB_YES = 1
TARGET_IDEF     = $(TARGET_NAME)implib.$(EXT_DEF)
ALL_INCLUDES    = -I../kLib/include -I../include
ALL_DEFINES     = -DKKRNLLIB
MAKEFILE        = $(TARGET_NAME).mak
SUBDIRS         = src kLib\src
#SUBDIRS_DEP    = $(SUBDIRS) include
PREMAKEFILES    = clib16.mak devhelp16.mak

TARGET_OBJS =\
$(PATH_OBJ)\kKrnlLib_first.$(EXT_LIB)\kKrnlLib_first.$(EXT_LIB)\
$(PATH_OBJ)\kKrnlLib_src.$(EXT_LIB)\kKrnlLib_src.$(EXT_LIB)\

TARGET_LIBS =\
$(PATH_OBJ)\kLibR0.$(EXT_LIB)\kLibR0.$(EXT_LIB)\
os2386p.$(EXT_LIB)\
os2386.$(EXT_LIB)\
os2286p.$(EXT_LIB)\
!if "$(BUILD_ENV)" == "VAC308"
cppon30.$(EXT_LIB)\
!else
!if "$(BUILD_ENV)" == "VAC365"
cpprni36.$(EXT_LIB)\
!else
$(LIB_C_OBJ)\
!endif
!endif
$(PATH_OBJ)\kKrnlLib_devhelp.$(EXT_LIB)\kKrnlLib_devhelp.$(EXT_LIB)\
$(PATH_OBJ)\kKrnlLib_clib16.$(EXT_LIB)\kKrnlLib_clib16.$(EXT_LIB)\
$(PATH_OBJ)\kKrnlLib_last.$(EXT_LIB)\kKrnlLib_last.$(EXT_LIB)

TARGET_DEPS =\
$(PATH_OBJ)\kLibR0.$(EXT_LIB)\kLibR0.$(EXT_LIB)\
$(PATH_OBJ)\kKrnlLib_devhelp.$(EXT_LIB)\kKrnlLib_devhelp.$(EXT_LIB)\
$(PATH_OBJ)\kKrnlLib_clib16.$(EXT_LIB)\kKrnlLib_clib16.$(EXT_LIB)\
$(PATH_OBJ)\kKrnlLib_last.$(EXT_LIB)\kKrnlLib_last.$(EXT_LIB)

#
# Rule config.
#
RULES_FORWARD = $(TARGET).$(EXT_ILIB) $(TARGET_IDEF) $(TARGET_ILIB)
!include $(MAKE_INCLUDE_PROCESS)

!if !$(BUILD_FORWARDING)

#
# Generate import definition file.
#
$(TARGET_IDEF): $(PATH_TOOLS)\MkCallTab.$(EXT_EXE)
    $** deffile > $@

# alias for generating the import library.
$(TARGET_NAME).$(EXT_ILIB): $(TARGET_ILIB)

!endif #!BUILD_FORWARDING
