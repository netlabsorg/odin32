# $Id: setup.os2allrc.mk,v 1.2 2002-09-14 23:35:26 bird Exp $

MAKE_INCLUDE_RC_SETUP = $(PATH_MAKE)\setup.os2allrc.mk

#
# The tool - OS/2 RC
#
!if "$(BUILD_ENVS_BASE_POST:toolkit45=x)$(BUILD_ENVS_BASE_PRE:toolkit45=x)" != "$(BUILD_ENVS_BASE_POST)$(BUILD_ENVS_BASE_PRE)"
# New Toolkit (v45 or newer) - use old rc.
RC = rc16.exe
RL = rc16.exe
!else
# Old Toolkit (if any)
RC = rc.exe
RL = rc.exe
!endif

#
# The flags
#
RC_FLAGS = -r -n -DOS2 \
!if "$(BUILD_MODE)" != "RELEASE"
           -DDEBUG \
!endif
           $(RC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(RC_INCLUDES:-I=-i ) $(ALL_INCLUDES:-I=-i ) -i $(PATH_INCLUDES:;= -i )
RL_FLAGS = -x2 -n

