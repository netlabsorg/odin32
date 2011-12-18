# $Id: Makefile,v 1.13 2002-12-19 01:49:07 bird Exp $

#
# kKrnlLib/dev32 makefile.
#
# Copyright (c) 1998-2002 knut st. osmundsen (bird@anduin.net)
#
# GPL
#

#
# Setup config
#
ALL_INCLUDES    = -I../include -I../kLib/include
ALL_DEFINES     = -DKKRNLLIB
PATH_ROOT       = ..\..\..\..
!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)

#
# Target config
#
TARGET_MODE     = SYSLIB
TARGET_NAME     = src
TARGET_SUB      = kKrnlLib

PREMAKEFILES_NEEDED =\
    MkCallTab.mak\

PREMAKEFILES_LIB    =\
#    newdbg.mak \
#    newrel.mak \
    Dev16.mak \

POSTMAKEFILES_LIB =\
    first.mak \
    last.mak \
#    srcR3Tst.mak \

TARGET_OBJS     =\
$(PATH_TARGET)\d32Globals.$(EXT_OBJ)\
$(PATH_TARGET)\d32Hlp.$(EXT_OBJ)\
$(PATH_TARGET)\d32Init.$(EXT_OBJ)\
$(PATH_TARGET)\log.$(EXT_OBJ)\
\
$(PATH_TARGET)\env.$(EXT_OBJ)\
$(PATH_TARGET)\krnlInit.$(EXT_OBJ)\
$(PATH_TARGET)\krnlImportTable.$(EXT_OBJ)\
$(PATH_TARGET)\krnlLoadKernelSym.$(EXT_OBJ)\
$(PATH_TARGET)\krnlLoadKernelSymFile.$(EXT_OBJ)\
$(PATH_TARGET)\krnlLockedWrite.$(EXT_OBJ)\
$(PATH_TARGET)\krnlOverloading.$(EXT_OBJ)\
$(PATH_TARGET)\krnlPrologs.$(EXT_OBJ)\
$(PATH_TARGET)\krnlStubs.$(EXT_OBJ)\
$(PATH_TARGET)\krnlWrappers.$(EXT_OBJ)\
$(PATH_TARGET)\OS2KPTDA.$(EXT_OBJ)\
$(PATH_TARGET)\OS2KTCB.$(EXT_OBJ)\
\
$(PATH_TARGET)\abort.$(EXT_OBJ)\
$(PATH_TARGET)\fltused.$(EXT_OBJ)\
$(PATH_TARGET)\malloc.$(EXT_OBJ)\
$(PATH_TARGET)\rmalloc.$(EXT_OBJ)\
$(PATH_TARGET)\smalloc.$(EXT_OBJ)\
$(PATH_TARGET)\stricmp.$(EXT_OBJ)\
$(PATH_TARGET)\vprintf.$(EXT_OBJ)\
$(PATH_TARGET)\vsprintf.$(EXT_OBJ)\
#$(PATH_OBJ)\$(TARGET_SUB)\newdbg.$(EXT_LIB)\newdbg.$(EXT_LIB)\
#$(PATH_OBJ)\$(TARGET_SUB)\newrel.$(EXT_LIB)\newrel.$(EXT_LIB)\
\
$(PATH_ROOT)\obj\$(SHT_TRGPLTFRM)$(SHT_BLDMD)mscv6_16\$(TARGET_SUB)\dev16.$(EXT_LIB)\dev16.$(EXT_LIB)\
\
$(PATH_TARGET)\SymDB32.$(EXT_OBJ)\
$(PATH_TARGET)\calltaba.$(EXT_OBJ)\

TARGET_NO_DEP   =\
calltaba.c \
calltaba.asm

#
# Rules config
#
RULES_FORWARD   = calltaba.asm
!include $(MAKE_INCLUDE_PROCESS)

!if !$(BUILD_FORWARDING)

#
# Generate calltaba.asm.
#
calltaba.asm: $(PATH_TOOLS)\MkCallTab.$(EXT_EXE)
    $** calltab > $@

!endif #!BUILD_FORWARDING

