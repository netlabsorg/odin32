# $Id: GetDriveType.mak,v 1.1 2002-04-13 04:38:54 bird Exp $
#
# GetDriveType.exe makefile.
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#
# Project Odin Software License can be found in LICENSE.TXT
#
#


#
# Pre Setup Config.
#

#
# Include Setup.
#
PATH_ROOT=..\..\..
!include $(PATH_ROOT)\make\setup.mak

#
# Post Setup Config
#
TARGET_NAME = GetDriveType
MAKEFILE    = $(TARGET_NAME).mak


#
# Targets and libraries (if non default)
#
TARGET_LIBS = \
$(LIB_OS)\
$(PATH_LIB)\kernel32.lib\
$(LIB_C_OBJ)



#
# Include Processing Rules.
#
!include $(MAKE_INCLUDE_PROCESS)


