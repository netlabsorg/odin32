# $Id: setup.os2relwat11.mk,v 1.8 2002-08-28 04:42:05 bird Exp $

#
# Note! Watcom is unable to do debug info release builds.
#       Because of a compiler bug which makes it emit code with
#       no optimizations.
#       Another reason is that lxlite isn't able to strip it.
#

# ---OS2, RELEASE, WAT11-------------------------
ENV_NAME="OS/2, Release, Watcom C/C++ v11.0c"
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
!include $(PATH_MAKE)\setup.os2relalp.mk
!include $(PATH_MAKE)\setup.os2relrc.mk
!include $(PATH_MAKE)\setup.os2relwrc.mk
!include $(PATH_MAKE)\setup.os2relwlink.mk
!ifdef LD_USE_ILINK
LD_OLDCPP       = 1
! include $(PATH_MAKE)\setup.os2relilink.mk
!else
_LD_LIBPATH     = $(PATH_WATCOM)\lib386\os2;$(PATH_WATCOM)\lib386;
! include $(PATH_MAKE)\setup.os2relwlink.mk
!endif
!include $(PATH_MAKE)\setup.optional.watcom11x.mk


#
# The tools
#
AR=ilib.exe
CC=wcc386.exe
CXX=wpp386.exe
LINK=wlink.exe
IMPLIB=implib.exe


#
# The flags
#
AR_FLAGS=/nologo /noignorecase
AR_CMD=$(AR) $(AR_FLAGS) @"$(TARGET_LNK)"
AR_LNK1= "$(@R).$(EXT_LIB)"
AR_LNK2=y
_AR_LNK3= +"$(TARGET_OBJS: ="&^
 +")"
AR_LNK3= $(_AR_LNK3:+""&^
=)
AR_LNK4= "$(@R).lst";

CC_FLAGS=-bt=os2v2 -dOS2 -d__32BIT__ -d__i386__ -5r -zq -bm -ze -w4 -zld $(_CC_OPTIONAL) $(CC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CC_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES) -i=$(WATCOM)\h
CC_FLAGS_EXE=$(CC_FLAGS) -omlinear -zc
CC_FLAGS_DLL=$(CC_FLAGS) -omlinear -zc -bd
CC_FLAGS_SYS=$(CC_FLAGS) -omlinear -s -zdp -zff -zgf -zu
CC_FLAGS_VDD=$(CC_FLAGS_SYS)
CC_FLAGS_IFS=$(CC_FLAGS_SYS) -omlnaru -bd
CC_OBJ_OUT=-fo=
CC_LST_OUT=
CC_PC_2_STDOUT=-pc

CXX_FLAGS=-bt=os2v2 -dOS2 -d__32BIT__ -d__i386__ -5r -zq -bm -ze -w4 -zld $(_CXX_OPTIONAL)  $(CXX_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CXX_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES) -i=$(WATCOM)\h
CXX_FLAGS_EXE=$(CXX_FLAGS) -omlinear -zc
CXX_FLAGS_DLL=$(CXX_FLAGS) -omlinear -zc -bd
CXX_FLAGS_SYS=$(CXX_FLAGS) -omlinear -omlinear -s -zdp -zff -zgf -zu
CXX_FLAGS_VDD=$(CXX_FLAGS_SYS)
CXX_FLAGS_IFS=$(CXX_FLAGS_SYS) -omlnaru -bd
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

IMPLIB_FLAGS=/NOI /Nologo


#
# Libraries and object files.
#
LIB_OS      = os2386.lib
!if "$(_CXX_XCPT)" == "-xd"
LIB_C_OBJ   = clib3r.lib plibmt3r.lib math387r.lib emu387.lib
LIB_C_DLL   = clbrdll.lib plbrdll.lib mt7rdll.lib emu387.lib
LIB_C_RTDLL = clbrdll.lib  # TODO
LIB_C_NRE   = $(LIB_C_OBJ) # TODO
!else
LIB_C_OBJ   = clib3r.lib plbxmt3r.lib math387r.lib emu387.lib
LIB_C_DLL   = clbrdll.lib plbrdllx.lib mt7rdll.lib emu387.lib
LIB_C_RTDLL = clbrdll.lib  # TODO
LIB_C_NRE   = $(LIB_C_OBJ) # TODO
!endif
LIB_C_DMNGL =
OBJ_PROFILE =

# ---OS2, RELEASE, WAT11-------------------------

