# $Id: last.mak,v 1.5 2002-08-20 05:04:43 bird Exp $

#
# kKrnlLib_last.lib makefile.
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

