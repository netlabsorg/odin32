# $Id: Makefile,v 1.7 2002-08-20 04:37:59 bird Exp $

#
# kKrnlLib/dev32 makefile.
#
#

#
# Load the build setup.
#
PATH_ROOT = ..\..\..\..
!include $(PATH_ROOT)\make\setup.mak


#
# Config.
#
TARGET_MODE  = SYSLIB
TARGET_NAME  = kKrnlLib_src
ALL_INCLUDES = -I../include -I../kLib/include
ALL_DEFINES  = -DKKRNLLIB


#
# Targets.
#
PREMAKEFILES = MkCallTab.mak newdbg.mak newrel.mak Dev16.mak

POSTMAKEFILES = first.mak last.mak srcR3Tst.mak

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
$(PATH_OBJ)\kKrnlLib_newdbg.$(EXT_LIB)\kKrnlLib_newdbg.$(EXT_LIB)\
$(PATH_OBJ)\kKrnlLib_newrel.$(EXT_LIB)\kKrnlLib_newrel.$(EXT_LIB)\
\
$(PATH_ROOT)\obj\$(NAME_1)$(NAME_2)mscv6\kKrnlLib_dev16.$(EXT_LIB)\kKrnlLib_dev16.$(EXT_LIB)\
\
$(PATH_TARGET)\SymDB32.$(EXT_OBJ)\
$(PATH_TARGET)\calltaba.$(EXT_OBJ)\


TARGET_DEPS =\
$(PATH_OBJ)\kKrnlLib_newdbg.$(EXT_LIB)\kKrnlLib_newdbg.$(EXT_LIB)\
$(PATH_OBJ)\kKrnlLib_newrel.$(EXT_LIB)\kKrnlLib_newrel.$(EXT_LIB)\
$(PATH_ROOT)\obj\$(NAME_1)$(NAME_2)mscv6\kKrnlLib_dev16.$(EXT_LIB)\kKrnlLib_dev16.$(EXT_LIB)\

TARGET_NO_DEP=\
calltaba.c


#
# Load the build process rules.
#
!include $(MAKE_INCLUDE_PROCESS)


# generated code.
calltaba.asm: .force
    $(TOOL_DOMAKES) "MkCallTab.mak" $(TOOL_MAKE) $@

