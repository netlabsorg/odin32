# $Id: setup.win32debwat11.mk,v 1.13 2002-08-30 19:44:51 bird Exp $

# ---WIN32, DEBUG, WAT11-------------------------
ENV_NAME="Win32, Debug, Watcom C/C++ v11.0c"
ENV_STATUS=OK
!if "$(ENV_ENVS)" == ""
ENV_ENVS=vac308 watcomc11c
!else
ENV_ENVS_FORCE=vac308 watcomc11c
!endif


#
# Include some shared standard stuff: ALP, optional stuff.
#
AS_DEBUG_TYPE = Codeview
!include $(PATH_MAKE)\setup.os2as.mk
!include $(PATH_MAKE)\setup.os2allrc.mk
!include $(PATH_MAKE)\setup.os2allwrc.mk
!ifdef LD_USE_ILINK
! error error: LD_USE_ILINK isn't supported by the win32 watcom setup.
!else
# TODO!
_LD_LIBPATH     = $(PATH_WATCOM)\lib386\nt;$(PATH_WATCOM)\lib386;
! include $(PATH_MAKE)\setup.os2relwlink.mk
!endif
!include $(PATH_MAKE)\setup.optional.watcom11x.mk


#
# The tools
#
AR=ilib.exe
CC=wcc386.exe
CXX=wpp386.exe
IMPLIB=implib.exe


#
# The flags
#
AR_FLAGS=/nologo /noignorecase
AR_CMD=$(AR) $(AR_FLAGS) $@ @$(TARGET_LNK)
_AR_LNK1= "$(TARGET_OBJS: ="&^
")"
AR_LNK1= $(_AR_LNK1:""=)
AR_LNK2= $(@R).lst

CC_FLAGS=-bt=os2v2 -dDEBUG -dWIN32 -d__32BIT__ -d__i386__ -zq -fr=nul -bm -ze -w4 -d2 -hc -zc $(_CC_OPTIONAL) $(CC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CC_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES) -i=$(WATCOM)\h
CC_FLAGS_EXE=$(CC_FLAGS)
CC_FLAGS_DLL=$(CC_FLAGS) -bd
CC_FLAGS_SYS=$(CC_FLAGS) -s -zdp -zff -zgf
CC_FLAGS_VDD=$(CC_FLAGS_SYS)
CC_FLAGS_IFS=$(CC_FLAGS_SYS) -bd
CC_OBJ_OUT=-fo=
CC_LST_OUT=
CC_PC_2_STDOUT=-pc

CXX_FLAGS=-bt=os2v2 -dDEBUG -dWIN32 -d__32BIT__ -d__i386__ -zq -fr=nul -bm -ze -w4 -d2 -hc -zc (_CXX_OPTIONAL)  $(CXX_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CXX_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES) -i=$(WATCOM)\h
CXX_FLAGS_EXE=$(CXX_FLAGS)
CXX_FLAGS_DLL=$(CXX_FLAGS) -bd
CXX_FLAGS_SYS=$(CXX_FLAGS) -s -zdp -zff -zgf
CXX_FLAGS_VDD=$(CXX_FLAGS_SYS)
CXX_FLAGS_IFS=$(CXX_FLAGS_SYS) -bd
CXX_OBJ_OUT=-fo=
CXX_LST_OUT=
CXX_PC_2_STDOUT=-pc

IMPLIB_FLAGS=/NOI /Nologo


#
# Libraries and object files.
#
LIB_OS      = $(WATCOM)\lib386\nt\kernel32.lib $(WATCOM)\lib386\nt\user32.lib $(WATCOM)\lib386\nt\gdi32.lib $(WATCOM)\lib386\nt\nt.lib
LIB_C_OBJ   = clib3r.lib
LIB_C_DLL   = clbrdll.lib
LIB_C_RTDLL = clbrdll.lib
LIB_C_NRE   = clib3r.lib
LIB_C_DMNGL =
OBJ_PROFILE =

# ---WIN32, DEBUG, WAT11-------------------------

