# $Id: first.mak,v 1.2 2002-03-31 19:01:16 bird Exp $

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
MAKEFILE    = first.mak
ALL_INCLUDES= -I../include
ALL_DEFINES = -DKKRNLLIB


#
# Object files.
#
TARGET_OBJS =\
$(PATH_TARGET)\devFirst.$(EXT_OBJ)\


#
# Load the build process rules.
#
!include $(MAKE_INCLUDE_PROCESS)

