# $Id: kKrnlLibR3Tst.mak,v 1.1 2002-03-31 19:30:39 bird Exp $

#
# kKrnlLib - Ring3 Test suite for the driver.
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
TARGET_MODE = EXE
TARGET_NAME = kKrnlLibR3Tst
TARGET_IGNORE_LINKER_WARNINGS=1
MAKEFILE    = $(TARGET_NAME).mak
ALL_INCLUDES= -I../include
ALL_DEFINES = -DKKRNLLIB -DR3TST


#
# Object files.
#
#$(PATH_TARGET)\stub.$(EXT_OBJ)\
TARGET_OBJS =\
$(PATH_OBJ)\kKrnlLib_first.$(EXT_LIB)\kKrnlLib_first.$(EXT_LIB)\
$(PATH_OBJ)\kKrnlLib_testcase.$(EXT_LIB)\kKrnlLib_testcase.$(EXT_LIB)\
$(PATH_OBJ)\kKrnlLib_srcR3Tst.$(EXT_LIB)\kKrnlLib_srcR3Tst.$(EXT_LIB)\

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

SUBDIRS     = include src kLib testcase

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
# Generate calltaba.asm
#
CallTaba.asm:
    $(PATH_TOOLS)\MkCallTab.exe calltab > $@


#
# kKrnlLib Import library.
#
$(TARGET).lib: $(PATH_LIB)\$(TARGET).lib
$(PATH_LIB)\$(TARGET).lib: $(PATH_TOOLS)\MkCallTab.exe
    $(PATH_TOOLS)\MkCallTab.exe deffile > $(PATH_TARGET)\$(TARGET)implib.def
    $(IMPLIB) $(IMPLIB_FLAGS) $@ $(PATH_TARGET)\$(TARGET)implib.def



$(PATH_TOOLS)\MkCallTab.exe: force
    $(PATH_)

