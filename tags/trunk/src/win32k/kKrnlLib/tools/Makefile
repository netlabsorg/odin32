# $Id: Makefile,v 1.3 2002-08-22 02:09:43 bird Exp $

#
# Makefile for all tools.
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
TARGET_NAME     = clfix
TARGET_PUBNAME  = $(PATH_TOOLS)\$(TARGET_NAME).$(TARGET_EXT)
TARGET_NEEDED   = 1
ALL_INCLUDES    = -I../include
POSTMAKEFILES   = ignore.mak libconv.mak

#
# Rules config
#
!include $(MAKE_INCLUDE_PROCESS)

