# $Id: first.mak,v 1.1.2.1 2002-04-01 09:06:54 bird Exp $

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
TARGET_MODE = SYSLIB
TARGET_NAME = Win32k_first
MAKEFILE    = first.mak
ALL_INCLUDES= -I../include -I../include/kKrnlLib
ALL_DEFINES = -DWIN32K


#
# Object files.
#
TARGET_OBJS =\
$(PATH_TARGET)\devFirst.$(EXT_OBJ)\


#
# Load the build process rules.
#
!include $(MAKE_INCLUDE_PROCESS)

