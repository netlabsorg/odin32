# $Id: newdbg.mak,v 1.1 2002-03-10 02:45:57 bird Exp $

#
# newdbg - separate make file for the debug new.
#
# Copyright (c) 1998-2002 knut st. osmundsen (bird@anduin.net)
#
# Project Odin Software License can be found in LICENSE.TXT
#


#
# Setup.
#
ALL_INCLUDES= -I..\include\kLib -I..\include
ALL_DEFINES = -DKKRNLLIB
PATH_ROOT   = ..\..\..\..
!include $(PATH_ROOT)\tools\make\setup.mak
!if "$(BUILD_ENV)" == "VAC308" || "$(BUILD_ENV)" == "VAC365"
CXX_FLAGS_SYS = $(CXX_FLAGS_SYS) -Tm+
!endif


#
# Config.
#
TARGET_MODE = SYSLIB
TARGET_NAME = kKrnlLib_newdbg


#
# Object files.
#
TARGET_OBJS =\
$(PATH_TARGET)\newdbg.$(EXT_OBJ)


#
# Process
#
!include $(MAKE_INCLUDE_PROCESS)

