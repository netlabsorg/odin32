# $Id: setup.os2allmasm510.mk,v 1.2 2002-08-30 20:28:18 bird Exp $

#
# The tool(s)
#
AS              = masm

#
# The flags
#
AS_FLAGS        = -DOS2 \
!if "$(BUILD_MODE)" != "RELEASE"
                  -DDEBUG \
!endif
                  -z -Zd -Zi -Mx -t $(BUILD_DEFINES) $(ALL_DEFINES) $(AS_DEFINES) \
                  $(AS_INCLUDES) $(ALL_INCLUDES) -I$(PATH_INCLUDES:;= -I)
AS_FLAGS_SYS    = $(AS_FLAGS) -DRING0

AS_CMD          = echo . && set MASM=$(AS_FLAGS:  = ) && $(AS) $(_SRC), $(_DST);
AS_CMD_SYS      = echo . && set MASM=$(AS_FLAGS_SYS:  = ) && $(AS) $(_SRC), $(_DST);


