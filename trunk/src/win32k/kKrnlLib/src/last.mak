# $Id: last.mak,v 1.6 2002-08-24 22:07:41 bird Exp $

#
# devLast.lib makefile.
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
PATH_ROOT       = ..\..\..\..
!include $(PATH_ROOT)\make\setup.mak

#
# target config.
#
TARGET_MODE     = SYSLIB
TARGET_NAME     = devLast
TARGET_SUB      = kKrnlLib
MAKEFILE        = last.mak

#
# Rules config.
#
!include $(MAKE_INCLUDE_PROCESS)

