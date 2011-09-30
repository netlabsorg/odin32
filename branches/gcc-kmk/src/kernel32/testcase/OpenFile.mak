# $Id: OpenFile.mak,v 1.1 2003-05-06 10:11:44 sandervl Exp $
#
# OpenFile testcases.
#
# Copyright (c) 2003 InnoTek Systemberatung GmbH
# Author: knut st. osmundsen <bird@anduin.net>
#
# Project Odin Software License can be found in LICENSE.TXT.
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
ALL_DEFINES = -DORIGINAL_VAC_FUNCTIONS 


#
# Post Setup Config
#
TARGET_NAME = OpenFile
TARGET_MODE	= EXE
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
    @$(ECHO) $(CLRFIL)OpenFile$(CLRTXT) - Running testcases.$(CLRRST)
    @$(TS_EXEC_1) "OF_PARSE - non existing file."                    $(TS_EXEC_2) testcase-1
    @$(TS_EXEC_1) "OF_PARSE - existing file. (the executable)"       $(TS_EXEC_2) testcase-2
    @$(TS_EXEC_1) "OF_PARSE - non existing dir and file."            $(TS_EXEC_2) testcase-3
    @$(TS_EXEC_1) "OF_PARSE - non existing file with searching."     $(TS_EXEC_2) testcase-4
    @$(TS_EXEC_1) "OF_PARSE - non existing file with searching."     $(TS_EXEC_2) testcase-4
    @$(TS_EXEC_1) "OF_PARSE - non existing file + other OF_s."       $(TS_EXEC_2) testcase-5
    @$(ECHO) $(CLRFIL)OpenFile$(CLRTXT) - Testcases Completed.$(CLRRST)

#
# All in one rule.
# (Makefiles are fun! :-)
#
testcase-1 testcase-2 testcase-3 testcase-4 testcase-5:
    @$(TARGET) $(@:testcase-=)

