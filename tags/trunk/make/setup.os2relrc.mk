# $Id: setup.os2relrc.mk,v 1.1 2002-08-27 03:58:02 bird Exp $

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
RC_FLAGS=-r -n -DOS2 $(RC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(RC_INCLUDES:-I=-i ) $(ALL_INCLUDES:-I=-i ) -i $(PATH_INCLUDES:;= -i )
RL_FLAGS=-x2 -n

