# $Id: setup.os2debalp.mk,v 1.2 2002-04-13 04:39:59 bird Exp $

#
# The tool(s)
#
AS          = alp

#
# The flags
#
AS_FLAGS    = -Mb -Sv:ALP +Od -D:DEBUG -D:OS2 $(AS_DEFINES:-D=-D:) $(ALL_INCLUDES:-I=-i:) $(AS_INCLUDES:-I=-i:) $(ALL_DEFINES:-D=-D:) -i:$(PATH_INCLUDES) $(BUILD_DEFINES:-D=-D:)
AS_FLAGS_SYS= $(AS_FLAGS) -D:RING0 -i:$(PATH_DDK)\base\inc
AS_OBJ_OUT  = -Fo:

