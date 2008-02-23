# $Id: dll-c.mak,v 1.3 2002-08-20 21:14:29 bird Exp $

# Testcase for makeing simple C DLLs.

# root & setup
!if "$(BUILD_ENV)" == "WAT11C-16" || "$(BUILD_ENV)" == "MSCV6-16"
ALL_MODEL=LARGE
!endif
TARGET_MODE = DLL #watcom+msc6-16 workaround (again!)
PATH_ROOT=..\..
!include $(PATH_ROOT)\make\setup.mak

# target config.
TARGET_NAME = dll-c
MAKEFILE    = dll-c.mak
TARGET_LIBS = $(LIB_C_OBJ) $(LIB_OS)    # use the static library!

# rules and more
RULES_FORWARD = test
!include $(MAKE_INCLUDE_PROCESS)

!if !$(BUILD_FORWARDING)



# some helpers for the output.
!if "$(BUILD_MODE)" == "DEBUG" || "$(BUILD_MODE)" == "PROFILE"
_STR1 = Hello "C" "DEBUG" World!
!else
_STR1 = Hello "C" World!
!endif

!ifdef ENV_16BIT
_STR2 = __16BIT__
!else
_STR2 = __32BIT__
!endif


#
# validate that the executable actually runs and produces the desired output.
#
OUTFILE=out
test:
    $(TOOL_MAKE) -f dll-prog-c.mak rebuild
    $(PATH_OBJ)\dll-prog-c.exe\dll-prog-c.exe $(TARGET) > $(OUTFILE)
! ifdef BUILD_VERBOSE
    type $(OUTFILE)
! endif
    $(TOOL_CMP) $(OUTFILE) <<
DLLInit
$(_STR1)
$(_STR2)
FOOName
DLLTerm
<<
    $(TOOL_MAKE) -f dll-prog-c.mak clean

!endif

