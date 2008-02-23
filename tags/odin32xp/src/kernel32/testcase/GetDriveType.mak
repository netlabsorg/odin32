# $Id: GetDriveType.mak,v 1.4 2002-06-26 07:11:03 sandervl Exp $
#
# GetDriveType.exe makefile.
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
TARGET_NAME = GetDriveType
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
# The testcase Rules.
#
!if "$(ODIN32_TESTCASE_DRIVE_FIXED)" == "" || "$(ODIN32_TESTCASE_DRIVE_FLOPPY)" == "" || "$(ODIN32_TESTCASE_DRIVE_UNUSED)" == ""
testcase:
    @$(TOOL_TYPE)  <<
Can't run testcases as no ODIN32_TESTCASE_DRIVE_* environment variables are configured.
    ODIN32_TESTCASE_DRIVE_UNUSED=<letter of unused drive>
    ODIN32_TESTCASE_DRIVE_FLOPPY=<letter of floppy drive>
    ODIN32_TESTCASE_DRIVE_FIXED=<letter of a fixed disk drive>
    ODIN32_TESTCASE_DRIVE_CDROM=<letter of cdrom drive>         (optional)
    ODIN32_TESTCASE_DRIVE_NETWORK=<letter of network drive>     (optional)
    ODIN32_TESTCASE_DRIVE_RAMDISK=<letter of ramdisk drive>     (optional)

Note: No colon!
<<
!else

testcase:
    @$(ECHO) $(CLRFIL)GetDriveType$(CLRTXT) - Running testcases.$(CLRRST)
    $(TS_EXEC_1) "Plain" $(TS_EXEC_2) testcase-1
    $(TS_EXEC_1) "Complex" $(TS_EXEC_2) testcase-2
    $(TS_EXEC_1) "Negative" $(TS_EXEC_2) testcase-2
    @$(ECHO) $(CLRFIL)GetDriveType$(CLRTXT) - Testcases Completed.$(CLRRST)

testcase-1:
# plain 'c:' styled input. check trice in case of buffering bugs.
    $(TARGET) $(ODIN32_TESTCASE_DRIVE_UNUSED):=DRIVE_NO_ROOT_DIR $(ODIN32_TESTCASE_DRIVE_UNUSED):=DRIVE_NO_ROOT_DIR $(ODIN32_TESTCASE_DRIVE_UNUSED):=DRIVE_NO_ROOT_DIR
    $(TARGET) $(ODIN32_TESTCASE_DRIVE_FLOPPY):=DRIVE_REMOVABLE   $(ODIN32_TESTCASE_DRIVE_FLOPPY):=DRIVE_REMOVABLE   $(ODIN32_TESTCASE_DRIVE_FLOPPY):=DRIVE_REMOVABLE
    $(TARGET) $(ODIN32_TESTCASE_DRIVE_FIXED):=DRIVE_FIXED        $(ODIN32_TESTCASE_DRIVE_FIXED):=DRIVE_FIXED        $(ODIN32_TESTCASE_DRIVE_FIXED):=DRIVE_FIXED
!if "$(ODIN32_TESTCASE_DRIVE_CDROM)" != ""
    $(TARGET) $(ODIN32_TESTCASE_DRIVE_CDROM):=DRIVE_CDROM        $(ODIN32_TESTCASE_DRIVE_CDROM):=DRIVE_CDROM        $(ODIN32_TESTCASE_DRIVE_CDROM):=DRIVE_CDROM
!endif
!if "$(ODIN32_TESTCASE_DRIVE_NETWORK)" != ""
    $(TARGET) $(ODIN32_TESTCASE_DRIVE_NETWORK):=DRIVE_REMOTE     $(ODIN32_TESTCASE_DRIVE_NETWORK):=DRIVE_REMOTE     $(TARGET) $(ODIN32_TESTCASE_DRIVE_NETWORK):=DRIVE_REMOTE
!endif
!if "$(ODIN32_TESTCASE_DRIVE_RAMDISK)" != ""
    $(TARGET) $(ODIN32_TESTCASE_DRIVE_RAMDISK):\=DRIVE_RAMDISK   $(ODIN32_TESTCASE_DRIVE_RAMDISK):\=DRIVE_RAMDISK   $(ODIN32_TESTCASE_DRIVE_RAMDISK):\=DRIVE_RAMDISK
!endif
# plain 'c:\' styled input (don't test all)
    $(TARGET) $(ODIN32_TESTCASE_DRIVE_UNUSED):\=DRIVE_NO_ROOT_DIR
    $(TARGET) $(ODIN32_TESTCASE_DRIVE_FIXED):\=DRIVE_FIXED
# plain 'c:/' styled input (don't test all)
    $(TARGET) $(ODIN32_TESTCASE_DRIVE_UNUSED):/=DRIVE_NO_ROOT_DIR
    $(TARGET) $(ODIN32_TESTCASE_DRIVE_FIXED):/=DRIVE_FIXED

testcase-2:
# complex input (but still ok)
    $(TARGET) $(ODIN32_TESTCASE_DRIVE_UNUSED):/\/\/\...=DRIVE_NO_ROOT_DIR
    $(TARGET) $(ODIN32_TESTCASE_DRIVE_FIXED):/\/\/\...=DRIVE_FIXED
    $(TARGET) $(ODIN32_TESTCASE_DRIVE_UNUSED):..............=DRIVE_NO_ROOT_DIR
    $(TARGET) $(ODIN32_TESTCASE_DRIVE_FIXED):..............=DRIVE_FIXED
    $(TARGET) $(ODIN32_TESTCASE_DRIVE_UNUSED):..............\\\\=DRIVE_NO_ROOT_DIR
    $(TARGET) $(ODIN32_TESTCASE_DRIVE_FIXED):.......\\/.\\.//.....\.\\\\=DRIVE_FIXED
    $(TARGET) $(ODIN32_TESTCASE_DRIVE_FIXED):..............\\.\\\\=DRIVE_FIXED
    $(TARGET) "$(ODIN32_TESTCASE_DRIVE_FIXED):..............\\.\\\\"=DRIVE_FIXED
    $(TARGET) "$(ODIN32_TESTCASE_DRIVE_FIXED):\\\........\\.\\\\"=DRIVE_FIXED

testcase-3:
# negative testcases
    $(TARGET) "."=DRIVE_NO_ROOT_DIR "9"=DRIVE_NO_ROOT_DIR "+"=DRIVE_NO_ROOT_DIR "-"=DRIVE_NO_ROOT_DIR
    $(TARGET) "$(ODIN32_TESTCASE_DRIVE_FIXED)"=DRIVE_NO_ROOT_DIR
    $(TARGET) "$(ODIN32_TESTCASE_DRIVE_FIXED)\\"=DRIVE_NO_ROOT_DIR
    $(TARGET) "$(ODIN32_TESTCASE_DRIVE_FIXED)\ \\"=DRIVE_NO_ROOT_DIR
    $(TARGET) "$(ODIN32_TESTCASE_DRIVE_FIXED):\\ \ \ \ \ \\\\\"=DRIVE_NO_ROOT_DIR

    @$(ECHO) $(CLRFIL)GetDriveType$(CLRTXT) - Testcases Completed.$(CLRRST)
!endif
