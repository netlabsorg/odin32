# $Id: setup.os2relalp.mk,v 1.5 2002-08-29 10:01:45 bird Exp $

#
# The tool(s)
#
AS          = alp

#
# The flags
#
!include $(PATH_MAKE)\setup.optional.alp.mk
AS_FLAGS    = -D:OS2 $(_AS_OPTIONAL) -Mb $(BUILD_DEFINES:-D=-D:) $(ALL_DEFINES:-D=-D:) $(AS_DEFINES:-D=-D:) $(AS_INCLUDES:-I=-i:) $(ALL_INCLUDES:-I=-i:) -i:$(PATH_INCLUDES)
AS_FLAGS_SYS= $(AS_FLAGS) -D:RING0
AS_OBJ_OUT= -Fo:

