# $Id: setup.os2debwat11.mk,v 1.22 2003-11-14 22:02:37 bird Exp $

# ---OS2, DEBUG, WAT11-------------------------
ENV_NAME="OS/2, Debug, Watcom C/C++ v11.0c"
ENV_STATUS=OK
!if "$(ENV_ENVS)" == ""
ENV_ENVS=vac308 watcomc11c
!else
ENV_ENVS_FORCE=vac308 watcomc11c
!endif
!undef ENV_16BIT


#
# Include some shared standard stuff: ALP, VAC optional stuff.
#
AS_DEBUG_TYPE   = Codeview
!include $(PATH_MAKE)\setup.os2as.mk
!include $(PATH_MAKE)\setup.os2allrc.mk
!include $(PATH_MAKE)\setup.os2allwrc.mk
_LD_LIBPATH     = $(PATH_WATCOM)\lib386\os2;$(PATH_WATCOM)\lib386;
LD_OLDCPP       = 1
!include $(PATH_MAKE)\setup.os2ld.mk
!include $(PATH_MAKE)\setup.os2ar.mk
!include $(PATH_MAKE)\setup.os2allipfc.mk
!include $(PATH_MAKE)\setup.optional.watcom11x.mk


#
# The tools
#
CC=wcc386.exe
CXX=wpp386.exe


#
# The flags
#
CC_FLAGS=-bt=os2v2 -dDEBUG -dOS2 -d__32BIT__ -d__i386__ -zq -fr=nul -bm -ze -w4 -zld -d2 -hc $(_CC_OPTIONAL) $(CC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CC_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES) -i=$(WATCOM)\h
CC_FLAGS_EXE=$(CC_FLAGS) -zc
CC_FLAGS_DLL=$(CC_FLAGS) -zc -bd
CC_FLAGS_SYS=$(CC_FLAGS) -s -zdp -zff -zgf -zu -dRING0
CC_FLAGS_VDD=$(CC_FLAGS_SYS)
CC_FLAGS_IFS=$(CC_FLAGS_SYS) -bd

CXX_FLAGS=-bt=os2v2 -dDEBUG -dOS2 -d__32BIT__ -d__i386__ -zq -fr=nul -bm -ze -w4 -zld -d2 -hc $(_CXX_OPTIONAL)  $(CXX_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CXX_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES) -i=$(WATCOM)\h
CXX_FLAGS_EXE=$(CXX_FLAGS) -zc
CXX_FLAGS_DLL=$(CXX_FLAGS) -zc -bd
CXX_FLAGS_SYS=$(CXX_FLAGS) -s -zdp -zff -zgf -zu -dRING0
CXX_FLAGS_VDD=$(CXX_FLAGS_SYS)
CXX_FLAGS_IFS=$(CXX_FLAGS_SYS) -bd

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
#
LIB_OS      = os2386.lib
!if "$(_CXX_XCPT)" == "-xd"
LIB_C_OBJ   = $(PATH_WATCOM)\lib386\os2\clib3r.lib $(PATH_WATCOM)\lib386\os2\plibmt3r.lib $(PATH_WATCOM)\lib386\plibmt3r.lib $(PATH_WATCOM)\lib386\math387r.lib $(PATH_WATCOM)\lib386\os2\emu387.lib
LIB_C_DLL   = $(PATH_WATCOM)\lib386\os2\clbrdll.lib $(PATH_WATCOM)\lib386\os2\plbrdll.lib $(PATH_WATCOM)\lib386\plbrdll.lib $(PATH_WATCOM)\lib386\os2\mt7rdll.lib $(PATH_WATCOM)\lib386\os2\emu387.lib
LIB_C_RTDLL = $(PATH_WATCOM)\lib386\os2\clbrdll.lib  # TODO
LIB_C_NRE   = $(LIB_C_OBJ) # TODO
!else
LIB_C_OBJ   = $(PATH_WATCOM)\lib386\os2\clib3r.lib $(PATH_WATCOM)\lib386\os2\plbxmt3r.lib $(PATH_WATCOM)\lib386\plbxmt3r.lib $(PATH_WATCOM)\lib386\math387r.lib $(PATH_WATCOM)\lib386\os2\emu387.lib
LIB_C_DLL   = $(PATH_WATCOM)\lib386\os2\clbrdll.lib $(PATH_WATCOM)\lib386\os2\plbrdllx.lib $(PATH_WATCOM)\lib386\plbrdllx.lib $(PATH_WATCOM)\lib386\os2\mt7rdll.lib $(PATH_WATCOM)\lib386\os2\emu387.lib
LIB_C_RTDLL = $(PATH_WATCOM)\lib386\os2\clbrdll.lib  # TODO
LIB_C_NRE   = $(LIB_C_OBJ) # TODO
!endif
LIB_C_DMNGL =
OBJ_PROFILE =

# ---OS2, DEBUG, WAT11-------------------------

