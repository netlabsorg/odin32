# $Id: setup.win32allalp.mk,v 1.1 2003-06-30 13:19:50 bird Exp $
# TODO sync this over to the master version.

#
# The tool(s)
#
AS          = alp

#
# Options
#
!ifdef AS_MODE
_AS_MODE =
! if "$(AS_MODE)" == "ALP"
_AS_MODE = -Sv:ALP
! endif
! if "$(AS_MODE)" == "M510"
_AS_MODE = -Sv:M510
! endif
! if "$(AS_MODE)" == "M600"
_AS_MODE = -Sv:M600
! endif
! if "$(_AS_MODE)" == ""
!  if [$(ECHO) $(CLRERR)Error: Bad AS_MODE ($(AS_MODE)). Valid modes are ALP, M510 and M600.$(CLRRST)]
!  endif
!  error
! endif
!else
_AS_MODE = -Sv:ALP
!endif

_AS_DEBUG_TYPE = +Od:IBM32
!ifdef AS_DEBUG_TYPE
! if "$(AS_DEBUG_TYPE)" == "Codeview"
_AS_DEBUG_TYPE = +Od:MS16
! else
!  if "$(AS_DEBUG_TYPE)" != "HLL"
!   if [$(ECHO) $(CLRERR)Error: Bad AS_DEBUG_TYPE ($(AS_DEBUG_TYPE)). Valid types are Codeview and HLL.$(CLRRST)]
!   endif
!   error
!  endif
! endif
!endif

_AS_OPTIONAL = $(_AS_MODE) $(_AS_DEBUG_TYPE)


#
# The flags
#
AS_FLAGS    = -D:WIN32 \
!if "$(BUILD_MODE)" != "RELEASE"
              -D:DEBUG \
!endif
               -Mb $(_AS_OPTIONAL) \
               $(BUILD_DEFINES:-D=-D:) $(ALL_DEFINES:-D=-D:) $(AS_DEFINES:-D=-D:)\
               $(AS_INCLUDES:-I=-i:) $(ALL_INCLUDES:-I=-i:) -i:$(PATH_INCLUDES)
AS_FLAGS_SYS= $(AS_FLAGS) -D:RING0
AS_OBJ_OUT  = -Fo:

