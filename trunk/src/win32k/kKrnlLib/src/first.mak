# $Id: first.mak,v 1.1 2002-03-10 02:45:55 bird Exp $

#
# kKrnlLib/dev32 makefile.
#
#

#
# Load the build setup.
#
PATH_ROOT = ..\..\..\..
!include $(PATH_ROOT)\tools\make\setup.mak


#
# Config.
#
TARGET_MODE = SYSLIB
TARGET_NAME = kKrnlLib_first
C_INCLUDES  = -I../include/kLib -I../include
AS_INCLUDES = -I../include/kLib -I../include
CC_DEFINES  = -DKKRNLLIB
CXX_DEFINES = -DKKRNLLIB


#
# Object files.
#
TARGET_OBJS =\
$(PATH_TARGET)\devFirst.$(EXT_OBJ)\


#
# Load the build process rules.
#
!include $(MAKE_INCLUDE_PROCESS)

