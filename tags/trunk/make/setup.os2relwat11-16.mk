# $Id: setup.os2relwat11-16.mk,v 1.12 2002-08-28 05:02:22 bird Exp $

#
# Note! Watcom is unable to do debug info release builds.
#       Because of a compiler bug which makes it emit code with
#       no optimizations.
#       Another reason is that lxlite isn't able to strip it.
#

# ---OS2, RELEASE, WAT11-------------------------
ENV_NAME="OS/2, Release, Watcom C/C++ v11.0c 16-bit"
ENV_STATUS=OK
!if "$(ENV_ENVS)" == ""
ENV_ENVS=ddkbase vac308 watcomc11c-16
!else
ENV_ENVS_FORCE=ddkbase vac308 watcomc11c-16
!endif
ENV_16BIT = 16


#
# Include some shared standard stuff: ALP, WRC, VAC optional stuff.
#
AS_DEBUG_TYPE   = Codeview
!include $(PATH_MAKE)\setup.os2relalp.mk
!include $(PATH_MAKE)\setup.os2relrc.mk
!include $(PATH_MAKE)\setup.os2relwrc.mk
!ifdef LD_USE_ILINK
LD_OLDCPP       = 1
! include $(PATH_MAKE)\setup.os2relilink.mk
!else
_LD_LIBPATH     = $(PATH_WATCOM)\lib286\os2;$(PATH_WATCOM)\lib286;
! include $(PATH_MAKE)\setup.os2relwlink.mk
!endif
!include $(PATH_MAKE)\setup.os2allilib.mk
!include $(PATH_MAKE)\setup.optional.watcom11x.mk


#
# The tools
#
CC=wcc.exe
CXX=wpp.exe


#
# The flags
#
_CC_FLAGS_OS =

CC_FLAGS=-bt=os2 -dOS2 -d__16BIT__ -5 -zq -bm -ze -w4 -zld $(_CC_OPTIONAL) $(CC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CC_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES) -i=$(WATCOM)\h
CC_FLAGS_EXE=$(CC_FLAGS) -omlinear -zc
CC_FLAGS_DLL=$(CC_FLAGS) -omlinear -zc -bd
CC_FLAGS_SYS=$(CC_FLAGS) -omlinear -s -zff -zgf -zu
CC_FLAGS_VDD=$(CC_FLAGS_SYS)
CC_FLAGS_IFS=$(CC_FLAGS) -out      -s -zff -zgf -zu -bd
CC_OBJ_OUT=-fo=
CC_LST_OUT=
CC_PC_2_STDOUT=-pc

CXX_FLAGS=-bt=os2 -dOS2 -d__16BIT__ -5 -zq -bm -ze -w4 -zld $(_CXX_OPTIONAL)  $(CXX_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CXX_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES) -i=$(WATCOM)\h
CXX_FLAGS_EXE=$(CXX_FLAGS) -omlinear -zc
CXX_FLAGS_DLL=$(CXX_FLAGS) -omlinear -zc -bd
CXX_FLAGS_SYS=$(CXX_FLAGS) -omlinear -s -zff -zgf -zu
CXX_FLAGS_VDD=$(CXX_FLAGS_SYS)
CXX_FLAGS_IFS=$(CXX_FLAGS) -omlnaru  -s -zff -zgf -zdp -zu -bd
CXX_OBJ_OUT=-fo=
CXX_LST_OUT=
CXX_PC_2_STDOUT=-pc

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


# ---OS2, RELEASE, WAT11-------------------------

