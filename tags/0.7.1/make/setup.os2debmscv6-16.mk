# $Id: setup.os2debmscv6-16.mk,v 1.16 2002-09-22 07:05:57 bird Exp $

# ---OS2, DEBUG, MSCV6-------------------------
ENV_NAME="OS/2, Debug, Microsoft C v6.0a 16-bit"
ENV_STATUS=OK
!if "$(ENV_ENVS)" == ""
ENV_ENVS=vac308 mscv6-16
!else
ENV_ENVS_FORCE=vac308 mscv6-16
!endif
SPACE=_
ENV_16BIT = 16


#
# Include some shared standard stuff: ALP.
#
!include $(PATH_MAKE)\setup.os2as.mk
!include $(PATH_MAKE)\setup.os2allrc.mk
!include $(PATH_MAKE)\setup.os2allwrc.mk
!include $(PATH_MAKE)\setup.os2ld.mk
!include $(PATH_MAKE)\setup.os2ar.mk
!include $(PATH_MAKE)\setup.os2allipfc.mk
!include $(PATH_MAKE)\setup.optional.mscvx-16.mk


#
# The tools
#
CC=echo . && set CL=$(CC_FLAGS) && cl.exe
CC=cl.exe
CXX=false


#
# The flags
#
CC_FLAGS=/nologo /c /DDEBUG /DOS2 /D__16BIT__ /Zi $(_CC_OPTIONAL) $(CC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CC_INCLUDES) $(ALL_INCLUDES) /I$(PATH_INCLUDES)
CC_FLAGS_EXE=$(CC_FLAGS)
CC_FLAGS_DLL=$(CC_FLAGS)
CC_FLAGS_SYS=$(CC_FLAGS) /DRING0 /Gs
CC_FLAGS_IFS=$(CC_FLAGS_SYS)
CC_OBJ_OUT=/Fo
CC_LST_OUT=/Fa
CC_PC_2_STDOUT=/P /E

CXX_FLAGS=
CXX_FLAGS_EXE=
CXX_FLAGS_DLL=
CXX_FLAGS_SYS=
CXX_FLAGS_IFS=
CXX_OBJ_OUT=
CXX_PC_2_STDOUT=

!if "$(CC_AS_CXX)" != ""
! if [@(ECHO) $(CLRERR)Error: CC_AS_CXX is not supported by this compiler!$(CLRRST)]
! endif
! error
!endif
!if "$(CXX_AS_CC)" != ""
! if [@(ECHO) $(CLRERR)Error: CXX_AS_CC is not supported by this compiler!$(CLRRST)]
! endif
! error
!endif


#
# Libraries and object files.
#
!if "$(TARGET_MODE)" == "DLL" # quirk! TODO/FIXME
_LIB_TYP = dll
!else
_LIB_TYP = ep
!endif
LIB_OS      = os2286.lib
LIB_C_OBJ   = $(_OBJ_MODEL)libc$(_LIB_TYP).lib
LIB_C_DLL   = $(_OBJ_MODEL)libc$(_LIB_TYP).lib
LIB_C_RTDLL = $(_OBJ_MODEL)libc$(_LIB_TYP).lib
LIB_C_NRE   =
LIB_C_DMNGL =
OBJ_PROFILE =

# ---OS2, DEBUG, MSCV6-------------------------

