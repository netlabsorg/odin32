# $Id: setup.os2debwrc.mk,v 1.1 2002-08-24 04:38:32 bird Exp $

#
# The tool - Odin32 RC
#
ORC = $(PATH_TOOLS)\wrc.exe -M

#
# The flags
#
ORC_FLAGS   = -DOS2 -DDEBUG $(ORC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES:-D__WINE__=) \
              $(ALL_INCLUDES) $(ORC_INCLUDES) -I$(PATH_INCLUDES) -I$(INCLUDE) \
!ifndef ORC_NO_PREFIX
              -p$(TARGET_NAME)
!endif

ORC_ASM_OUT = -s -o

