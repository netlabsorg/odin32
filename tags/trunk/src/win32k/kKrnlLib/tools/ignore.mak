# $Id: ignore.mak,v 1.2 2002-04-07 22:40:16 bird Exp $

#
# Makefile for the ignore tool.
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#


#
# Setup.
#
PATH_ROOT = ..\..\..\..
!include $(PATH_ROOT)\make\setup.mak


#
# Config.
#
TARGET_NAME = ignore
MAKEFILE    = $(TARGET_NAME).mak
ALL_INCLUDES= -I../include/kLib -I../include
PATH_BIN    = $(PATH_TOOLS)

#
# Process
#
!include $(MAKE_INCLUDE_PROCESS)

