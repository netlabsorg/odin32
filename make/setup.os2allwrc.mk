# $Id: setup.os2allwrc.mk,v 1.1 2002-08-30 18:27:22 bird Exp $

#
# The tool - Odin32 RC
#
ORC = $(PATH_TOOLS)\wrc.exe

#
# The flags
#
ORC_FLAGS   = -DOS2 \
!if "$(BUILD_MODE)" != "RELEASE"
              -DDEBUG \
!endif              
              $(ORC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES:-D__WINE__=) \
              $(ALL_INCLUDES) $(ORC_INCLUDES) -I$(PATH_INCLUDES) \
              -I$(PATH_VAC308)\include;$(PATH_VAC365)\include;$(PATH_WATCOM)\include \
!ifndef ORC_NO_PREFIX
              -p$(TARGET_NAME)
!endif

ORC_ASM_OUT = -s -o

