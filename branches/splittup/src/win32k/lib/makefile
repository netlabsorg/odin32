# $Id: makefile,v 1.14.2.4 2002-04-21 23:38:48 bird Exp $

#
# Odin32 API
#
#       win32k.lib makefile
#


#
# Load the build setup.
#
PATH_ROOT = ..\..\..
!include $(PATH_ROOT)\make\setup.mak


#
# Config.
#
TARGET_MODE = LIB
TARGET_PUBLIC  = 1
TARGET_NAME = Win32k
ALL_DEFINES = -DRING3 -DWIN32KLIB


#
# Object files.
#
TARGET_OBJS =\
$(PATH_TARGET)\libInit.$(EXT_OBJ)\
$(PATH_TARGET)\libTerm.$(EXT_OBJ)\
$(PATH_TARGET)\libWin32kInstalled.$(EXT_OBJ)\
$(PATH_TARGET)\libWin32kQueryOptionsStatus.$(EXT_OBJ)\
$(PATH_TARGET)\libWin32kSetOptions.$(EXT_OBJ)\
$(PATH_TARGET)\libWin32kSetEnvironment.$(EXT_OBJ)\
$(PATH_TARGET)\libDosAllocMemEx.$(EXT_OBJ)\
$(PATH_TARGET)\libDosKillProcessEx.$(EXT_OBJ)\
$(PATH_TARGET)\libW32kHandleSystemEvent.$(EXT_OBJ)\
$(PATH_TARGET)\libW32kQueryOTEs.$(EXT_OBJ)\
$(PATH_TARGET)\libW32kQuerySystemMemInfo.$(EXT_OBJ)\
$(PATH_TARGET)\libW32kProcessReadWrite.$(EXT_OBJ)\
$(PATH_TARGET)\libGetCS.$(EXT_OBJ)\
$(PATH_TARGET)\libCallThruCallGate.$(EXT_OBJ)\


#
# Load the build process rules.
#
!include $(MAKE_INCLUDE_PROCESS)

