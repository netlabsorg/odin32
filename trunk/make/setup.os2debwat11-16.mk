# $Id: setup.os2debwat11-16.mk,v 1.20 2003-11-14 22:02:36 bird Exp $

# ---OS2, DEBUG, WAT11-------------------------
ENV_NAME="OS/2, Debug, Watcom C/C++ v11.0c 16-bit"
ENV_STATUS=OK
!if "$(ENV_ENVS)" == ""
ENV_ENVS=vac308 watcomc11c-16
!else
ENV_ENVS_FORCE=vac308 watcomc11c-16
!endif
ENV_16BIT = 16


#
# Include some shared standard stuff: ALP, VAC optional stuff.
#
AS_DEBUG_TYPE   = Codeview
!include $(PATH_MAKE)\setup.os2as.mk
!include $(PATH_MAKE)\setup.os2allrc.mk
!include $(PATH_MAKE)\setup.os2allwrc.mk
LD_OLDCPP       = 1
_LD_LIBPATH     = $(PATH_WATCOM)\lib286\os2;$(PATH_WATCOM)\lib286;
!include $(PATH_MAKE)\setup.os2ld.mk
!include $(PATH_MAKE)\setup.os2ar.mk
!include $(PATH_MAKE)\setup.os2allipfc.mk
!include $(PATH_MAKE)\setup.optional.watcom11x.mk


#
# The tools
#
CC=wcc.exe
CXX=wpp.exe


#
# The flags
#
CC_FLAGS=-bt=os2 -dOS2 -d__16BIT__ -dDEBUG -zq -fr=nul -bm -ze -w4 -zld -d2 -hc $(_CC_OPTIONAL) $(CC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CC_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES) -i=$(WATCOM)\h
CC_FLAGS_EXE=$(CC_FLAGS) -zc
CC_FLAGS_DLL=$(CC_FLAGS) -zc -bd
CC_FLAGS_SYS=$(CC_FLAGS) -s -zff -zgf -zu -dRING0
CC_FLAGS_VDD=$(CC_FLAGS_SYS)
CC_FLAGS_IFS=$(CC_FLAGS) -s -zff -zgf -zu -bd -dRING0

CXX_FLAGS=-bt=os2 -dOS2 -d__16BIT__ -dDEBUG -zq -fr=nul -bm -ze -w4 -zld -d2 -hc $(_CXX_OPTIONAL)  $(CXX_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CXX_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES) -i=$(WATCOM)\h
CXX_FLAGS_EXE=$(CXX_FLAGS) -zc
CXX_FLAGS_DLL=$(CXX_FLAGS) -zc -bd
CXX_FLAGS_SYS=$(CXX_FLAGS) -s -zff -zgf -zu -dRING0
CXX_FLAGS_VDD=$(CXX_FLAGS_SYS)
CXX_FLAGS_IFS=$(CXX_FLAGS) -s -zff -zgf -zdp -zu -bd -dRING0

!if "$(CC_AS_CXX)" != ""
! if "$(CXX_AS_CC)" != ""
!  if [@(ECHO) $(CLRERR)Error: CC_AS_CXX and CXX_AS_CC is mutual execlusive!$(CLRRST)]
!  endif
!  error
! endif
CC=$(CXX)
CC_FLAGS_EXE=$(CXX_FLAGS_EXE)
CC_FLAGS_DLL=$(CXX_FLAGS_DLL)
CC_FLAGS_SYS=$(CXX_FLAGS_SYS)
CC_FLAGS_VDD=$(CXX_FLAGS_VDD)
CC_FLAGS_IFS=$(CXX_FLAGS_IFS)
!endif
!if "$(CXX_AS_CC)" != ""
CXX=$(CC)
CXX_FLAGS_EXE=$(CC_FLAGS_EXE)
CXX_FLAGS_DLL=$(CC_FLAGS_DLL)
CXX_FLAGS_SYS=$(CC_FLAGS_SYS)
CXX_FLAGS_VDD=$(CC_FLAGS_VDD)
CXX_FLAGS_IFS=$(CC_FLAGS_IFS)
!endif


#
# Libraries and object files.
# TODO - DLL and RTDLL doesn't work.
#        Probably needing code to be compiled with far code. I.e. MODEL hugh, large or medium.
#
LIB_OS      = os2286.lib

_LIB_MT =
!if "$(_OBJ_MODEL)" == "m"
_LIB_MT = mt # this is the only one there is a MT edition of, don't know why.
!endif
_LIB_DLL=
!if "$(TARGET_MODE)" == "DLL" # quirk! TODO/FIXME
_LIB_DLL= dl
!endif
!if "$(_CXX_XCPT)" != "-xs"
LIB_C_OBJ   = clib$(_LIB_DLL)$(_LIB_MT)$(_OBJ_MODEL).lib plib$(_LIB_MT)$(_OBJ_MODEL).lib math87$(_LIB_MT)$(_OBJ_MODEL).lib emu87.lib
!else
LIB_C_OBJ   = clib$(_LIB_DLL)$(_LIB_MT)$(_OBJ_MODEL).lib plbx$(_LIB_MT)$(_OBJ_MODEL).lib math87$(_LIB_MT)$(_OBJ_MODEL).lib emu87.lib
!endif
LIB_C_DLL   = $(LIB_C_OBJ)  # clibdll.lib
LIB_C_RTDLL = $(LIB_C_OBJ)  # clibdll.lib
LIB_C_NRE   = $(LIB_C_OBJ)
LIB_C_DMNGL =
OBJ_PROFILE =


# ---OS2, DEBUG, WAT11-------------------------

