# $Id: dll-c.mak,v 1.2 2002-08-20 07:56:24 bird Exp $

# Testcase for makeing simple C DLLs.

# root & setup
!if "$(BUILD_ENV)" == "WAT11C-16"
ALL_MODEL=LARGE
!endif
TARGET_MODE = DLL #watcom workaround (again!)
PATH_ROOT=..\..
!include $(PATH_ROOT)\make\setup.mak

# target config.
TARGET_NAME = dll-c
MAKEFILE    = dll-c.mak
TARGET_LIBS = $(LIB_C_OBJ) $(LIB_OS)    # use the static library!

# rules and more
!include $(MAKE_INCLUDE_PROCESS)



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
test:
!if "$(MAKE_INCLUDE_PROCESS:process.forwarder=)" == "$(MAKE_INCLUDE_PROCESS)"
    $(TOOL_MAKE) -f dll-prog-c.mak rebuild
    $(PATH_OBJ)\dll-prog-c.exe\dll-prog-c.exe $(TARGET) > out
! ifdef BUILD_VERBOSE
    type out
! endif
    $(TOOL_CMP) out <<
DLLInit
$(_STR1)
$(_STR2)
FOOName
DLLTerm
<<
    $(TOOL_MAKE) -f dll-prog-c.mak clean
!else
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@
!endif

