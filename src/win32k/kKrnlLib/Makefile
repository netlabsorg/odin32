# $Id: Makefile,v 1.33 2002-08-22 03:07:50 bird Exp $

#
# kKrnlLib main file.
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#
# GPL
#

#
# Setup.
#
PATH_ROOT = ..\..\..
!include $(PATH_ROOT)\makefile.inc
!include $(PATH_ROOT)\make\setup.mak


#
# Config.
#
TARGET_MODE = EMPTY
BUILD_OWN_TESTCASE_RULE = 1
!ifdef WITH_KLIB
SUBDIRS     = kLib\src
!endif

PREMAKEFILES=\
!ifdef WITH_KKRNLLIB
        kKrnlLib.mak \
#        kKrnlLibR3Tst.mak \
!endif
!ifdef WITH_KLIB
        kLibR3.mak \
!endif

!ifdef WITH_KKRNLLIB
SUBDIRS_NEEDED = tools
!endif
SUBDIRS_TESTCASE =



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

