# $Id: Makefile,v 1.2 2002-04-07 22:40:16 bird Exp $

#
# Makefile for all tools.
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
TARGET_NAME = clfix
ALL_INCLUDES= -I../include/kLib -I../include
PATH_BIN    = $(PATH_TOOLS)


#
# Object files.
#
TARGET_NAME  = clfix
PREMAKEFILES = ignore.mak libconv.mak


#
# Process
#
!include $(MAKE_INCLUDE_PROCESS)


#
# needed rulez
#
needed: build install

