# $Id: Makefile,v 1.26 2002-04-07 22:40:15 bird Exp $

#
# kKrnlLib main file.
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#


#
# Setup.
#
PATH_ROOT = ..\..\..
!include $(PATH_ROOT)\make\setup.mak


#
# Config.
#
TARGET_MODE = EMPTY
TARGET_NAME = kKrnlLib_empty
SUBDIRS     = tools include
PREMAKEFILES= kKrnlLib.mak kKrnlLibR3Tst.mak
BUILD_NO_TESTCASE = 1



#
# Process
#
!include $(MAKE_INCLUDE_PROCESS)


needed:
    @$(TOOL_DODIRS) "tools" $(TOOL_MAKE) build
#    @$(TOOL_DOMAKES) "" $(TOOL_MAKE) build


#
# Our own minimal testcase rule
#
testcase:
!if [$(TOOL_EXISTS) testcase] == 0
    @$(TOOL_DODIRS) "testcase" $(TOOL_MAKE) $@
!endif

