# $Id: Makefile,v 1.23 2002-03-10 04:58:29 bird Exp $

#
# kKrnlLib main file.
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#


#
# Setup.
#
PATH_ROOT = ..\..\..
!include $(PATH_ROOT)\tools\make\setup.mak


#
# Config.
#
TARGET_MODE = EMPTY
TARGET_NAME = kKrnlLib_empty
SUBDIRS     = tools include
PREMAKEFILES= kKrnlLib.mak #kKrnlLibR3.mak


#
# Process
#
!include $(MAKE_INCLUDE_PROCESS)


needed:
    @$(TOOL_DODIRS) "tools" $(TOOL_MAKE) build
#    @$(TOOL_DOMAKES) "" $(TOOL_MAKE) build

