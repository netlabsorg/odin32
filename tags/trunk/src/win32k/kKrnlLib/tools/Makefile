# $Id: Makefile,v 1.4 2002-12-19 01:49:10 bird Exp $

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
TARGET_PUB_DIR  = $(PATH_TOOLS)
TARGET_NEEDED   = 1
ALL_INCLUDES    = -I../include
POSTMAKEFILES_NEEDED   = ignore.mak libconv.mak

#
# Rules config
#
!include $(MAKE_INCLUDE_PROCESS)

