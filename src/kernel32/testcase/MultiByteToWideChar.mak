# $Id: MultiByteToWideChar.mak,v 1.1 2002-06-26 07:11:03 sandervl Exp $
#
# SetConsoleCtrlHandler.exe makefile.
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#
# Project Odin Software License can be found in LICENSE.TXT
#
#


#
# Pre Setup Config.
#


#
# Include Setup.
#
PATH_ROOT=..\..\..
!include $(PATH_ROOT)\make\setup.mak
!include $(PATH_MAKE)\testsuite.tools.mk


#
# Post Setup Config
#
TARGET_NAME = MultiByteToWideChar
MAKEFILE    = $(TARGET_NAME).mak
BUILD_OWN_TESTCASE_RULE = 1


#
# Targets and libraries (if non default)
#
TARGET_LIBS = \
$(LIB_OS)\
!if "$(BUILD_PLATFORM)" == "OS2"
$(PATH_LIB)\kernel32.lib\
!endif
$(LIB_C_OBJ)



#
# Include Processing Rules.
#
!include $(MAKE_INCLUDE_PROCESS)


#
# The testcase Rule.
#
testcase:
    @$(ECHO) $(CLRFIL)MultiByteToWideChar$(CLRTXT) - Running testcases.$(CLRRST)
    $(TS_EXEC_1) "Simple Strings." $(TS_EXEC_2) testcase-1
    @$(ECHO) $(CLRFIL)MultiByteToWideChar$(CLRTXT) - Testcases Completed.$(CLRRST)

testcase-1 testcase-2 testcase-3:
    $(TARGET) $(@:testcase-=)

