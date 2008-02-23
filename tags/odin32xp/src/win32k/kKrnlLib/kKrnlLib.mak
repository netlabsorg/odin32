# $Id: kKrnlLib.mak,v 1.7 2002-08-24 22:27:38 bird Exp $

#
# kKrnlLib.
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#


#
# Setup config.
#
ALL_INCLUDES    = -I../kLib/include -I../include
ALL_DEFINES     = -DKKRNLLIB
PATH_ROOT       = ..\..\..
!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)

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
TARGET_SUB      = kKrnlLib
TARGET_PUB_SUB  = $(PATH_SUB_SYS)
TARGET_ILIB_YES = 1
TARGET_IDEF     = $(TARGET_NAME)implib.$(EXT_DEF)
MAKEFILE        = $(TARGET_NAME).mak
SUBDIRS_NEEDED  = src
SUBDIRS_LIB     = src kLib\src
#SUBDIRS_DEP    = $(SUBDIRS) include
PREMAKEFILES_LIB= clib16.mak devhelp16.mak

TARGET_OBJS =\
$(PATH_OBJ)\$(TARGET_SUB)\devFirst.$(EXT_LIB)\devFirst.$(EXT_LIB)\
$(PATH_OBJ)\$(TARGET_SUB)\src.$(EXT_LIB)\src.$(EXT_LIB)\

TARGET_LIBS =\
$(PATH_OBJ)\kLibR0.$(EXT_LIB)\kLibR0.$(EXT_LIB)\
os2386p.$(EXT_LIB)\
os2386.$(EXT_LIB)\
os2286p.$(EXT_LIB)\
$(LIB_C_NRE)\
$(PATH_OBJ)\$(TARGET_SUB)\devhelp16.$(EXT_LIB)\devhelp16.$(EXT_LIB)\
$(PATH_OBJ)\$(TARGET_SUB)\clib16.$(EXT_LIB)\clib16.$(EXT_LIB)\
$(PATH_OBJ)\$(TARGET_SUB)\devLast.$(EXT_LIB)\devLast.$(EXT_LIB)

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
