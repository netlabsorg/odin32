# $Id: setup.os2allmasm510.mk,v 1.1 2002-08-30 18:27:22 bird Exp $

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
                  -Zd -Zi -Mx -t $(BUILD_DEFINES) $(ALL_DEFINES) $(AS_DEFINES) \
                  $(AS_INCLUDES) $(ALL_INCLUDES) -I$(PATH_INCLUDES:;= -I)
AS_FLAGS_SYS    = $(AS_FLAGS) -DRING0

AS_CMD          = echo . && set MASM=$(AS_FLAGS:  = ) && $(AS) $(_SRC), $(_DST);
AS_CMD_SYS      = echo . && set MASM=$(AS_FLAGS_SYS:  = ) && $(AS) $(_SRC), $(_DST);


