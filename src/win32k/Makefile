# $Id: Makefile,v 1.71.2.3 2002-04-07 22:40:36 bird Exp $

#
# kKrnlLib main file.
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#


#
# Setup.
#
PATH_ROOT = ..\..
!include $(PATH_ROOT)\make\setup.mak


#
# Config.
#
TARGET_MODE = EMPTY
TARGET_NAME = win32k_empty
SUBDIRS     = kKrnlLib tools include lib
SUBDIRS     = tools include lib
PREMAKEFILES= win32k.mak win32kR3Tst.mak
BUILD_NO_TESTCASE = 1


#
# Process
#
!include $(MAKE_INCLUDE_PROCESS)


needed:
    @$(TOOL_DODIRS) "tools" $(TOOL_MAKE) build


#
# Our own minimal testcase rule
#
testcase:
!if [$(TOOL_EXISTS) testcase] == 0
    @$(TOOL_DODIRS) "kKrnlLib testcase" $(TOOL_MAKE) $@
!endif

