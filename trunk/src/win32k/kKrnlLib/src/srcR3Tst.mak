# $Id: srcR3Tst.mak,v 1.6 2002-08-20 05:47:53 bird Exp $

#
# kKrnlLib/srcR3Tst makefile.
#
# Copyright (c) 1998-2002 knut st. osmundsen (bird@anduin.net)
#
# GPL
#

#
# Load the build setup.
#
PATH_ROOT = ..\..\..\..
!include $(PATH_ROOT)\make\setup.mak
!include ..\..\makefile.inc


#
# Config.
#
TARGET_MODE  = SYSLIB
TARGET_NAME  = kKrnlLib_srcR3Tst
MAKEFILE     = srcR3Tst.mak
ALL_INCLUDES = -I../include -I../kLib/include
AS_INCLUDES  = -I$(PATH_DDKBASE)\inc
ALL_DEFINES  = -DKKRNLLIB -DR3TST


#
# Targets.
#
PREMAKEFILES = newdbgR3Tst.mak newrelR3Tst.mak Dev16R3Tst.mak

TARGET_OBJS =\
$(PATH_TARGET)\d32Globals.$(EXT_OBJ)\
$(PATH_TARGET)\d32Hlp.$(EXT_OBJ)\
$(PATH_TARGET)\d32Init.$(EXT_OBJ)\
$(PATH_TARGET)\log.$(EXT_OBJ)\
\
$(PATH_TARGET)\env.$(EXT_OBJ)\
$(PATH_TARGET)\krnlInit.$(EXT_OBJ)\
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
\
$(PATH_TARGET)\tstDH.$(EXT_OBJ)\
$(PATH_TARGET)\tstDos.$(EXT_OBJ)\
$(PATH_TARGET)\tstDosA.$(EXT_OBJ)\
$(PATH_TARGET)\tstFake.$(EXT_OBJ)\
$(PATH_TARGET)\tstFakeA.$(EXT_OBJ)\
$(PATH_TARGET)\tstInit.$(EXT_OBJ)\
$(PATH_TARGET)\tstkKrnlLib.$(EXT_OBJ)\
\
$(PATH_OBJ)\kKrnlLib_newdbgR3Tst.$(EXT_LIB)\kKrnlLib_newdbgR3Tst.$(EXT_LIB)\
$(PATH_OBJ)\kKrnlLib_newrelR3Tst.$(EXT_LIB)\kKrnlLib_newrelR3Tst.$(EXT_LIB)\
\
$(PATH_ROOT)\obj\$(SHT_TRGPLTFRM)$(SHT_BLDMD)mscv6-16\kKrnlLib_dev16R3Tst.$(EXT_LIB)\kKrnlLib_dev16R3Tst.$(EXT_LIB)\
\
$(PATH_TARGET)\SymDB32.$(EXT_OBJ)\
$(PATH_TARGET)\calltaba.$(EXT_OBJ)\
$(PATH_TARGET)\TstFakers.$(EXT_OBJ)\


TARGET_DEPS =\
$(PATH_OBJ)\kKrnlLib_newdbgR3Tst.$(EXT_LIB)\kKrnlLib_newdbgR3Tst.$(EXT_LIB)\
$(PATH_OBJ)\kKrnlLib_newrelR3Tst.$(EXT_LIB)\kKrnlLib_newrelR3Tst.$(EXT_LIB)\
$(PATH_ROOT)\obj\$(SHT_TRGPLTFRM)$(SHT_BLDMD)mscv6-16\kKrnlLib_dev16.$(EXT_LIB)\kKrnlLib_dev16.$(EXT_LIB)\


TARGET_NO_DEP=\
calltaba.c\
TstFakers.c


#
# Load the build process rules.
#
!include $(MAKE_INCLUDE_PROCESS)


# generated code.
TstFakers.c: .force
    $(TOOL_DOMAKES) "MkCallTab.mak" $(TOOL_MAKE) $@

