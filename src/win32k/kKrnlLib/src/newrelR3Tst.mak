# $Id: newrelR3Tst.mak,v 1.1 2002-03-31 19:30:40 bird Exp $

#
# newrelR3Tst - separate make file for the release new (R3Tst build).
#
# Copyright (c) 1998-2002 knut st. osmundsen (bird@anduin.net)
#
# Project Odin Software License can be found in LICENSE.TXT
#


#
# Setup.
#
ALL_INCLUDES= -I../include
ALL_DEFINES = -DKKRNLLIB -DR3TST
PATH_ROOT   = ..\..\..\..
!include $(PATH_ROOT)\tools\make\setup.mak
!if "$(BUILD_ENV)" == "VAC308" || "$(BUILD_ENV)" == "VAC365"
CXX_FLAGS_SYS = $(CXX_FLAGS_SYS) -Tm-
!endif


#
# Config.
#
TARGET_MODE = SYSLIB
TARGET_NAME = kKrnlLib_newrelR3Tst
MAKEFILE    = newrelR3Tst.mak


#
# Object files.
#
TARGET_OBJS =\
$(PATH_TARGET)\newrel.$(EXT_OBJ)


#
# Process
#
!include $(MAKE_INCLUDE_PROCESS)

