# $Id: ignore.mak,v 1.1 2002-03-10 04:57:07 bird Exp $

#
# Makefile for the ignore tool.
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#


#
# Setup.
#
PATH_ROOT = ..\..\..\..
!include $(PATH_ROOT)\tools\make\setup.mak


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

