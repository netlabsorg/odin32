# $Id: newrelR3Tst.mak,v 1.4 2002-08-20 05:04:43 bird Exp $

#
# newrelR3Tst - separate make file for the release new (R3Tst build).
#
# Copyright (c) 1998-2002 knut st. osmundsen (bird@anduin.net)
#
# GPL
#


#
# Setup.
#
ALL_INCLUDES= -I../include -I../kLib/include
ALL_DEFINES = -DKKRNLLIB -DR3TST
!if "$(BUILD_ENV)" == "VAC308" || "$(BUILD_ENV)" == "VAC365"
ALL_NO_DBGMEM = 1
!endif
PATH_ROOT   = ..\..\..\..
!include $(PATH_ROOT)\make\setup.mak


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

