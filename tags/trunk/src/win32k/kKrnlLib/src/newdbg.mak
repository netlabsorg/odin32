# $Id: newdbg.mak,v 1.6 2002-08-24 22:07:41 bird Exp $

#
# newdbg - separate make file for the debug new.
#
# Copyright (c) 1998-2002 knut st. osmundsen (bird@anduin.net)
#
# GPL
#

#
# Setup config.
#
ALL_INCLUDES    = -I../include -I../kLib/include
ALL_DEFINES     = -DKKRNLLIB
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
TARGET_SUB      = kKrnlLib
MAKEFILE        = newdbg.mak

#
# Rules config.
#
!include $(MAKE_INCLUDE_PROCESS)

