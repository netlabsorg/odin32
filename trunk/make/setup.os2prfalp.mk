# $Id: setup.os2prfalp.mk,v 1.4 2002-08-27 03:02:52 bird Exp $

#
# The tool(s)
#
AS          = alp

#
# The flags
#
!include $(PATH_MAKE)\setup.optional.alp.mk
AS_FLAGS    = -D:OS2 -D:DEBUG $(_AS_OPTIONAL) -Mb $(AS_DEFINES:-D=-D:) $(ALL_INCLUDES:-I=-i:) $(AS_INCLUDES:-I=-i:) $(ALL_DEFINES:-D=-D:) -i:$(PATH_INCLUDES) $(BUILD_DEFINES:-D=-D:)
AS_FLAGS_SYS= $(AS_FLAGS) -D:RING0 -i:$(PATH_DDK)\base\inc
AS_OBJ_OUT  = -Fo:


