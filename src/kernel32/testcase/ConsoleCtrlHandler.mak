# $Id: ConsoleCtrlHandler.mak,v 1.1 2002-06-26 07:11:03 sandervl Exp $
#
# SetConsoleCtrlHandler and GenerateConsoleCtrlEvent testcases.
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
!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)
!include $(PATH_MAKE)\testsuite.tools.mk


#
# Post Setup Config
#
TARGET_NAME = ConsoleCtrlHandler
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
    @$(ECHO) $(CLRFIL)ConsoleCtrlHandler$(CLRTXT) - Running testcases.$(CLRRST)
    $(TS_EXEC_1) "Negative testing of GenerateConsoleCtrlEvent."    $(TS_EXEC_2) testcase-20
    $(TS_EXEC_1) "Negative testing of SetConsoleCtrlHandler."       $(TS_EXEC_2) testcase-21
    $(TS_EXEC_1) "Multiple handlers."                               $(TS_EXEC_2) testcase-22
    $(TS_EXEC_1) "Multiple handlers. Singel Removal."               $(TS_EXEC_2) testcase-23
    $(TS_EXEC_1) "Multiple handlers. Complex Removal."              $(TS_EXEC_2) testcase-24
    @$(ECHO) $(CLRFIL)ConsoleCtrlHandler$(CLRTXT) - Testcases Completed.$(CLRRST)

#
# All in one rule.
# (Makefiles are fun! :-)
#
testcase-20 testcase-21 testcase-22 testcase-23 testcase-24 testcase-1 testcase-2 testcase-3:
    $(TARGET) $(@:testcase-=)

