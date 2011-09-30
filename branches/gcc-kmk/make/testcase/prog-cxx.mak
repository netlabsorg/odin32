# $Id: prog-cxx.mak,v 1.3 2002-08-28 03:40:04 bird Exp $

# Testcase for makeing simple C++ programs.

# root & setup
PATH_ROOT=..\..
!include $(PATH_ROOT)\make\setup.mak

# target config.
TARGET_MODE = EXE
TARGET_NAME = prog-cxx
MAKEFILE    = prog-cxx.mak
TARGET_LIBS = $(LIB_C_OBJ) $(LIB_OS)    # use the static library!

# rules and more
RULES_FORWARD = test
!include $(MAKE_INCLUDE_PROCESS)

!if !$(BUILD_FORWARDING)


# some helpers for the output.
!if "$(BUILD_MODE)" == "DEBUG" || "$(BUILD_MODE)" == "PROFILE"
_STR1 = Hello "C++" "DEBUG" World!
!else
_STR1 = Hello "C++" World!
!endif

!ifdef ENV_16BIT
_STR2 = __16BIT__
!else
_STR2 = __32BIT__
!endif


#
# validate that the executable actually runs and produces the desired output.
#
test:
    $(TARGET) > out
! ifdef BUILD_VERBOSE
    type out
! endif
    $(TOOL_CMP) out <<
$(_STR1)

$(_STR2)

<<

!endif

