# $Id: Makefile,v 1.1 2002-03-10 04:57:19 bird Exp $

#
# Makefile for all tools.
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

