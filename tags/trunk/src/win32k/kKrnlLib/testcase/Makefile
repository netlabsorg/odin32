# $Id: Makefile,v 1.1 2002-03-31 19:30:41 bird Exp $

#
# kKrnlLib/testcase makefile.
#
#

#
# Load the build setup.
#
!include ..\..\Makefile.inc
PATH_ROOT = ..\..\..\..
!include $(PATH_ROOT)\tools\make\setup.mak


#
# Config.
#
TARGET_MODE  = SYSLIB
TARGET_NAME  = kKrnlLib_testcase
ALL_INCLUDES = -I../include
AS_INCLUDES  = -I$(PATH_DDKBASE)\inc
ALL_DEFINES  = -DKKRNLLIB -DR3TST


#
# Targets.
#
TARGET_OBJS =\
$(PATH_TARGET)\tstDH.$(EXT_OBJ)\
$(PATH_TARGET)\tstDos.$(EXT_OBJ)\
$(PATH_TARGET)\tstDosA.$(EXT_OBJ)\
$(PATH_TARGET)\tstFake.$(EXT_OBJ)\
$(PATH_TARGET)\tstFakeA.$(EXT_OBJ)\
$(PATH_TARGET)\tstInit.$(EXT_OBJ)\
$(PATH_TARGET)\tstkKrnlLib.$(EXT_OBJ)\


#
# Load the build process rules.
#
!include $(MAKE_INCLUDE_PROCESS)

