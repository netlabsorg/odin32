# $Id: setup.win32allwcrc.mk,v 1.1 2003-06-30 13:19:51 bird Exp $
# @TODO sync into master.

MAKE_INCLUDE_RC_SETUP = $(PATH_MAKE)\setup.win32allwcrc.mk

#
# The tool - Watcom RC targetting Win32
#
RC = $(PATH_WATCOM)\binw\wrc.exe
RL = $(PATH_WATCOM)\binw\wrc.exe

#
# The flags
#
RC_RES_OUT = -fo=
RC_FLAGS = -r -q -bt=nt -DWIN32 \
!if "$(BUILD_MODE)" != "RELEASE"
           -DDEBUG \
!endif
           $(RC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(RC_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES:;= -i=)
RL_FLAGS = -q -bt=nt

