# $Id: ignore.mak,v 1.4 2002-08-22 03:07:50 bird Exp $

#
# Makefile for the ignore tool.
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#


#
# Setup config
#
PATH_ROOT = ..\..\..\..
!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)

#
# Target config
#
TARGET_NAME     = ignore
TARGET_PUBNAME  = $(PATH_TOOLS)\$(TARGET_NAME).$(TARGET_EXT)
TARGET_NEEDED   = 1
ALL_INCLUDES    = -I../include
MAKEFILE        = $(TARGET_NAME).mak

#
# Rules config
#
!include $(MAKE_INCLUDE_PROCESS)

