# $Id: Makefile,v 1.1.2.1 2002-04-01 09:06:54 bird Exp $

#
# Win32k/dev32 makefile.
#
#

#
# Load the build setup.
#
PATH_ROOT = ..\..\..
!include $(PATH_ROOT)\tools\make\setup.mak


#
# Config.
#
TARGET_MODE  = SYSLIB
TARGET_NAME  = win32k_src
ALL_INCLUDES = -I../include -I../kKrnlLib/include
ALL_DEFINES  = -DWIN32K -D__WIN32OS2__


#
# Targets.
#
PREMAKEFILES = #Dev16.mak

POSTMAKEFILES = first.mak last.mak #srcR3Tst.mak

TARGET_OBJS =\
#$(PATH_TARGET)\api.$(EXT_OBJ)\
$(PATH_TARGET)\d32globals.$(EXT_OBJ)\
$(PATH_TARGET)\d32init.$(EXT_OBJ)\
$(PATH_TARGET)\d32Win32kIOCtl.$(EXT_OBJ)\
$(PATH_TARGET)\d32Win32kOpenClose.$(EXT_OBJ)\
#$(PATH_TARGET)\elf2lx.$(EXT_OBJ)\
$(PATH_TARGET)\k32AllocMemEx.$(EXT_OBJ)\
$(PATH_TARGET)\k32HandleSystemEvent.$(EXT_OBJ)\
$(PATH_TARGET)\k32KillProcessEx.$(EXT_OBJ)\
$(PATH_TARGET)\k32ProcessReadWrite.$(EXT_OBJ)\
$(PATH_TARGET)\k32QueryCallGate.$(EXT_OBJ)\
$(PATH_TARGET)\k32QueryOptionsStatus.$(EXT_OBJ)\
$(PATH_TARGET)\k32QueryOTEs.$(EXT_OBJ)\
$(PATH_TARGET)\k32QuerySystemMemInfo.$(EXT_OBJ)\
#$(PATH_TARGET)\k32SendSystemEvent.$(EXT_OBJ)\
$(PATH_TARGET)\k32SetEnvironment.$(EXT_OBJ)\
$(PATH_TARGET)\k32SetOptions.$(EXT_OBJ)\
$(PATH_TARGET)\ldr.$(EXT_OBJ)\
$(PATH_TARGET)\mmf.$(EXT_OBJ)\
$(PATH_TARGET)\ModuleBase.$(EXT_OBJ)\
$(PATH_TARGET)\myLDRGetProcAddr.$(EXT_OBJ)\
$(PATH_TARGET)\myLDRQAppType.$(EXT_OBJ)\
$(PATH_TARGET)\myVMAllocMem.$(EXT_OBJ)\
$(PATH_TARGET)\myldrCheckInternalName.$(EXT_OBJ)\
$(PATH_TARGET)\myldrClose.$(EXT_OBJ)\
$(PATH_TARGET)\myldrEnum32bitRelRecs.$(EXT_OBJ)\
$(PATH_TARGET)\myldrFindModule.$(EXT_OBJ)\
$(PATH_TARGET)\myldrGetFileName.$(EXT_OBJ)\
$(PATH_TARGET)\myldrOpen.$(EXT_OBJ)\
$(PATH_TARGET)\myldrOpenPath.$(EXT_OBJ)\
$(PATH_TARGET)\myldrOpenPatha.$(EXT_OBJ)\
$(PATH_TARGET)\myldrRead.$(EXT_OBJ)\
$(PATH_TARGET)\myldrSetVMflags.$(EXT_OBJ)\
$(PATH_TARGET)\myldrWasLoadModuled.$(EXT_OBJ)\
$(PATH_TARGET)\mytkExecPgm.$(EXT_OBJ)\
$(PATH_TARGET)\mytkStartProcess.$(EXT_OBJ)\
#$(PATH_TARGET)\myVR32AllocMem.$(EXT_OBJ)\
$(PATH_TARGET)\pe2lx.$(EXT_OBJ)\
$(PATH_TARGET)\PerTaskW32kData.$(EXT_OBJ)\
$(PATH_TARGET)\d32CallGate.$(EXT_OBJ)\
$(PATH_TARGET)\d32Events.$(EXT_OBJ)\
$(PATH_TARGET)\devfirst.$(EXT_OBJ)\
$(PATH_TARGET)\devlast.$(EXT_OBJ)\
\
$(PATH_ROOT)\obj\$(NAME_1)$(NAME_2)mscv6\win32k_dev16.$(EXT_LIB)\win32k_dev16.$(EXT_LIB)\


TARGET_DEPS =\
$(PATH_ROOT)\obj\$(NAME_1)$(NAME_2)mscv6\win32k_dev16.$(EXT_LIB)\win32k_dev16.$(EXT_LIB)\


#
# Load the build process rules.
#
!include $(MAKE_INCLUDE_PROCESS)

