# $Id: newdbgR3Tst.mak,v 1.5 2002-08-24 22:07:41 bird Exp $

#
# newdbgR3Tst - separate make file for the debug new (R3Tst build).
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
ALL_DBGMEM      = 1
!endif
PATH_ROOT       = ..\..\..\..
!include $(PATH_ROOT)\make\setup.mak


#
# Target config.
#
TARGET_MODE     = SYSLIB
TARGET_NAME     = newdbg
TARGET_SUB      = kKrnlLib\R3Tst
MAKEFILE        = newdbgR3Tst.mak

#
# Rules config.
#
!include $(MAKE_INCLUDE_PROCESS)

