# $Id: setup.os2relalp.mk,v 1.3 2002-05-16 11:37:04 bird Exp $

#
# The tool(s)
#
AS          = alp

#
# The flags
#
!include $(PATH_MAKE)\setup.optional.alp.mk
AS_FLAGS    = -D:OS2 $(_AS_OPTIONAL) -Mb +Od $(AS_DEFINES:-D=-D:) $(ALL_INCLUDES:-I=-i:) $(AS_INCLUDES:-I=-i:) $(ALL_DEFINES:-D=-D:) -i:$(PATH_INCLUDES) $(BUILD_DEFINES:-D=-D:)
AS_FLAGS_SYS= $(AS_FLAGS) -D:RING0 -i:$(PATH_DDK)\base\inc
AS_OBJ_OUT= -Fo:

