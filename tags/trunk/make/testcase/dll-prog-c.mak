# $Id: dll-prog-c.mak,v 1.4 2002-08-28 03:40:03 bird Exp $

# Testcase for makeing simple C DLLs.

# root & setup
PATH_ROOT=..\..
!include $(PATH_ROOT)\make\setup.mak

# target config.
TARGET_MODE = EXE
TARGET_NAME = dll-prog-c
MAKEFILE    = dll-prog-c.mak
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
OUTFILE = out
test:
    $(TOOL_MAKE) -f dll-c.mak rebuild
    $(TARGET) $(PATH_OBJ)\dll-c.dll\dll-c.dll > $(OUTFILE)
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
    $(TOOL_MAKE) -f dll-c.mak clean

!endif

