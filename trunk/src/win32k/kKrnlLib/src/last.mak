# $Id: last.mak,v 1.4 2002-08-20 04:37:59 bird Exp $

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
ALL_INCLUDES= -I../include -I../kLib/include
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

