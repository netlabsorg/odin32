# $Id: last.mak,v 1.3 2002-04-07 22:39:45 bird Exp $

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
TARGET_MODE = SYSLIB
TARGET_NAME = kKrnlLib_last
MAKEFILE    = last.mak
ALL_INCLUDES= -I../include
ALL_DEFINES = -DKKRNLLIB


#
# Object files.
#
TARGET_OBJS =\
$(PATH_TARGET)\devLast.$(EXT_OBJ)\



#
# Load the build process rules.
#
!include $(MAKE_INCLUDE_PROCESS)

