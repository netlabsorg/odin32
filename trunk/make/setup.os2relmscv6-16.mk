# $Id: setup.os2relmscv6-16.mk,v 1.15 2002-09-20 04:10:05 bird Exp $

# ---OS2, RELEASE, MSCV6-------------------------
ENV_NAME="OS/2, Release, Microsoft C v6.0a 16-bit"
ENV_STATUS=OK
!if "$(ENV_ENVS)" == ""
ENV_ENVS=vac308 mscv6-16
!else
ENV_ENVS_FORCE=vac308 mscv6-16
!endif
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
CC=cl.exe
CXX=false


#
# The flags
#
CC_FLAGS=/nologo /c /DOS2 /D__16BIT__ /Zi $(_CC_OPTIONAL) $(CC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CC_INCLUDES) $(ALL_INCLUDES) /I$(PATH_INCLUDES)
CC_FLAGS_EXE=$(CC_FLAGS)
CC_FLAGS_DLL=$(CC_FLAGS)
CC_FLAGS_SYS=$(CC_FLAGS) /DRING0 /Gs
CC_FLAGS_IFS=$(CC_FLAGS_SYS)
CC_OBJ_OUT=/Fo
CC_LST_OUT=/Fl
CC_PC_2_STDOUT=/P /C /E

CXX_FLAGS=
CXX_FLAGS_EXE=
CXX_FLAGS_DLL=
CXX_FLAGS_SYS=
CXX_FLAGS_IFS=
CXX_OBJ_OUT=
CXX_LST_OUT=
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
LIB_C_OBJ   = $(_OBJ_MODEL)libc$(_LIB_TYP).$(EXT_LIB)
LIB_C_DLL   = $(_OBJ_MODEL)libc$(_LIB_TYP).$(EXT_LIB)
LIB_C_RTDLL = $(_OBJ_MODEL)libc$(_LIB_TYP).$(EXT_LIB)
LIB_C_NRE   =
LIB_C_DMNGL =
OBJ_PROFILE =

# ---OS2, RELEASE, MSCV6-------------------------

