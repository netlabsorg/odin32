# $Id: kKrnlLib.mak,v 1.3 2002-04-01 12:43:10 bird Exp $

#
# kKrnlLib.
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#


#
# Setup.
#
PATH_ROOT = ..\..\..
!include $(PATH_ROOT)\tools\make\setup.mak


#
# kLib overrides variables.
#
!if [SET KLIB_INC=$(MAKEDIR)\kLib.inc]
!endif


#
# Config.
#
TARGET_MODE = SYS
TARGET_NAME = kKrnlLib
TARGET_ILIB = kKrnlLib.lib
TARGET_IDEF = kKrnlLibimplib.def
MAKEFILE    = $(TARGET_NAME).mak
C_INCLUDES  = -I../include/kLib -I../include
AS_INCLUDES = -I../include/kLib -I../include
AS_DEFINES  = -DKKRNLLIB


#
# Object files.
#
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

SUBDIRS     = include src kLib
PREMAKEFILES= clib16.mak devhelp16.mak



#
# Process
#
!include $(MAKE_INCLUDE_PROCESS)


#
# Commandline for working in kLib.
#
cmd:
    $(COMSPEC) /K

#
# kKrnlLib Import library.
#
$(TARGET).lib: $(PATH_LIB)\$(TARGET).lib
$(PATH_LIB)\$(TARGET).lib: $(PATH_TOOLS)\MkCallTab.exe
    $(PATH_TOOLS)\MkCallTab.exe deffile > $(PATH_TARGET)\$(TARGET)implib.def
    $(IMPLIB) $(IMPLIB_FLAGS) $@ $(PATH_TARGET)\$(TARGET)implib.def


#
# Generate import definition file.
#
$(TARGET_IDEF): .force
    $(TOOL_DODIRS) "src" $(TOOL_MAKE) -f MkCallTab.mak ..\$(TARGET_IDEF)
