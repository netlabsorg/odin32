# $Id: kKrnlLibR3Tst.mak,v 1.3 2002-08-24 22:27:38 bird Exp $

#
# kKrnlLib - Ring3 Test suite for the driver.
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#

#
# Setup config.
#
ALL_INCLUDES    = -I../include
ALL_DEFINES     = -DKKRNLLIB -DR3TST
PATH_ROOT       = ..\..\..
!include $(PATH_ROOT)\make\setup.mak

#
# kLib overrides variables.
#
!if [SET KLIB_INC=$(MAKEDIR)\kLib.inc]
!endif

#
# Target config.
#
TARGET_MODE     = EXE
TARGET_NAME     = kKrnlLib
TARGET_SUB      = kKrnlLib\R3Tst
TARGET_IGNORE_LINKER_WARNINGS=1
MAKEFILE        = $(TARGET_NAME).mak

#$(PATH_TARGET)\stub.$(EXT_OBJ)\
TARGET_OBJS     =\
$(PATH_OBJ)\$(TARGET_SUB)\..\devFirst.$(EXT_LIB)\devFirst.$(EXT_LIB)\
$(PATH_OBJ)\$(TARGET_SUB)\src.$(EXT_LIB)\src.$(EXT_LIB)\

TARGET_LIBS     =\
$(PATH_OBJ)\kLibR0.$(EXT_LIB)\kLibR0.$(EXT_LIB)\
os2386p.$(EXT_LIB)\
$(LIB_OS)\
os2286p.$(EXT_LIB)\
$(LIB_C_NRE)\
$(PATH_OBJ)\$(TARGET_SUB)\..\devhelp.$(EXT_LIB)\devhelp.$(EXT_LIB)\
$(PATH_OBJ)\$(TARGET_SUB)\..\clib16.$(EXT_LIB)\clib16.$(EXT_LIB)\
$(PATH_OBJ)\$(TARGET_SUB)\..\devLast.$(EXT_LIB)\devLast.$(EXT_LIB)

SUBDIRS_LIB     = src kLib

PREMAKEFILES_LIB= clib16.mak devhelp16.mak

#
# Rules config.
#
RULES_FORWARD = calltab.asm $(TARGET).lib $(PATH_TOOLS)\MkCallTab.exe
!include $(MAKE_INCLUDE_PROCESS)


!if !$(BUILD_FORWARDING)
# BUGBUG: mess which doesn't work!

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
!endif
