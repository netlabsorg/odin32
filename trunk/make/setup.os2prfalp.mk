# $Id: setup.os2prfalp.mk,v 1.1 2002-04-07 22:43:54 bird Exp $

#
# The tool(s)
#
AS          = alp

#
# The flags
#
AS_FLAGS    = -Mb -Sv:ALP +Od -D:DEBUG -D:OS2 $(AS_DEFINES:-D=-D:) $(ALL_INCLUDES:-I=-i:) $(AS_INCLUDES:-I=-i:) $(ALL_DEFINES:-D=-D:) -i:$(PATH_INCLUDE)
AS_FLAGS_SYS= $(AS_FLAGS) -D:RING0 -i:$(PATH_DDK)\base\inc
AS_OBJ_OUT  = -Fo:


