# $Id: newrelR3Tst.mak,v 1.5 2002-08-24 22:07:41 bird Exp $

#
# newrelR3Tst - separate make file for the release new (R3Tst build).
#
# Copyright (c) 1998-2002 knut st. osmundsen (bird@anduin.net)
#
# GPL
#

#
# Setup config.
#
ALL_INCLUDES    = -I../include -I../kLib/include
ALL_DEFINES     = -DKKRNLLIB -DR3TST
!if "$(BUILD_ENV)" == "VAC308" || "$(BUILD_ENV)" == "VAC365"
ALL_NO_DBGMEM   = 1
!endif
PATH_ROOT       = ..\..\..\..
!include $(PATH_ROOT)\make\setup.mak


#
# Target config.
#
TARGET_MODE     = SYSLIB
TARGET_NAME     = newrel
TARGET_SUB      = kKrnlLib\R3Tst
MAKEFILE        = newrelR3Tst.mak

#
# Rules config.
#
!include $(MAKE_INCLUDE_PROCESS)

