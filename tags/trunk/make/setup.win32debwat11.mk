# $Id: setup.win32debwat11.mk,v 1.16 2003-09-14 18:53:03 bird Exp $

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
!include $(PATH_MAKE)\setup.win32allwcrc.mk
#!include $(PATH_MAKE)\setup.os2allwrc.mk
!ifdef LD_USE_ILINK
! error error: LD_USE_ILINK isn't supported by the win32 watcom setup.
!else
# TODO!
_LD_LIBPATH     = $(PATH_WATCOM)\lib386\nt;$(PATH_WATCOM)\lib386;
TOOL_MAPSYM_TYPE= WLINK
! include $(PATH_MAKE)\setup.os2relwlink.mk
!endif
!include $(PATH_MAKE)\setup.optional.watcom11x.mk
!include $(PATH_MAKE)\setup.os2allwlib.mk


#
# The tools
#
CC=wcc386.exe
CXX=wpp386.exe
IMPLIB=implib.exe
TOOL_STRIP=


#
# The flags
#
CC_FLAGS=-bt=nt -dDEBUG -dWIN32 -d__32BIT__ -d__i386__ -zq -fr=nul -bm -ze -w4 -d2 -hc -zc $(_CC_OPTIONAL) $(CC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CC_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES) -i=$(WATCOM)\h\nt -i=$(WATCOM)\h
CC_FLAGS_EXE=$(CC_FLAGS)
CC_FLAGS_DLL=$(CC_FLAGS) -bd
CC_FLAGS_SYS=$(CC_FLAGS) -s -zdp -zff -zgf
CC_FLAGS_VDD=$(CC_FLAGS_SYS)
CC_FLAGS_IFS=$(CC_FLAGS_SYS) -bd

CXX_FLAGS=-bt=nt -dDEBUG -dWIN32 -d__32BIT__ -d__i386__ -zq -fr=nul -bm -ze -w4 -d2 -hc -zc $(_CXX_OPTIONAL)  $(CXX_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CXX_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES) -i=$(WATCOM)\h\nt -i=$(WATCOM)\h
CXX_FLAGS_EXE=$(CXX_FLAGS)
CXX_FLAGS_DLL=$(CXX_FLAGS) -bd
CXX_FLAGS_SYS=$(CXX_FLAGS) -s -zdp -zff -zgf
CXX_FLAGS_VDD=$(CXX_FLAGS_SYS)
CXX_FLAGS_IFS=$(CXX_FLAGS_SYS) -bd

IMPLIB_FLAGS=/NOI /Nologo


#
# Libraries and object files.
#
LIB_OS      = $(WATCOM)\lib386\nt\kernel32.lib $(WATCOM)\lib386\nt\user32.lib $(WATCOM)\lib386\nt\gdi32.lib $(WATCOM)\lib386\nt\advapi32.lib $(WATCOM)\lib386\nt\nt.lib
!if "$(_CXX_XCPT)" == "-xd"
LIB_C_OBJ   = $(PATH_WATCOM)\lib386\nt\clib3r.lib $(PATH_WATCOM)\lib386\nt\plibmt3r.lib $(PATH_WATCOM)\lib386\plibmt3r.lib $(PATH_WATCOM)\lib386\math387r.lib $(PATH_WATCOM)\lib386\nt\emu387.lib
LIB_C_DLL   = $(PATH_WATCOM)\lib386\nt\clbrdll.lib $(PATH_WATCOM)\lib386\nt\plbrdll.lib $(PATH_WATCOM)\lib386\plbrdll.lib $(PATH_WATCOM)\lib386\nt\mt7rdll.lib $(PATH_WATCOM)\lib386\nt\emu387.lib
LIB_C_RTDLL = $(PATH_WATCOM)\lib386\nt\clbrdll.lib  # TODO
LIB_C_NRE   = $(LIB_C_OBJ) # TODO
!else
LIB_C_OBJ   = $(PATH_WATCOM)\lib386\nt\clib3r.lib $(PATH_WATCOM)\lib386\nt\plbxmt3r.lib $(PATH_WATCOM)\lib386\plbxmt3r.lib $(PATH_WATCOM)\lib386\math387r.lib $(PATH_WATCOM)\lib386\nt\emu387.lib
LIB_C_DLL   = $(PATH_WATCOM)\lib386\nt\clbrdll.lib $(PATH_WATCOM)\lib386\nt\plbrdllx.lib $(PATH_WATCOM)\lib386\plbrdllx.lib $(PATH_WATCOM)\lib386\nt\mt7rdll.lib $(PATH_WATCOM)\lib386\nt\emu387.lib
LIB_C_RTDLL = $(PATH_WATCOM)\lib386\nt\clbrdll.lib  # TODO
LIB_C_NRE   = $(LIB_C_OBJ) # TODO
!endif
#LIB_C_OBJ   = clib3r.lib
#LIB_C_DLL   = clbrdll.lib
#LIB_C_RTDLL = clbrdll.lib
#LIB_C_NRE   = clib3r.lib
LIB_C_DMNGL =
OBJ_PROFILE =

# ---WIN32, DEBUG, WAT11-------------------------

