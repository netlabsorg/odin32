# $Id: setup.os2debwat11.mk,v 1.14 2002-08-27 03:58:01 bird Exp $

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
AS_DEBUG_TYPE = Codeview
!include $(PATH_MAKE)\setup.os2debalp.mk
!include $(PATH_MAKE)\setup.os2prfrc.mk
!include $(PATH_MAKE)\setup.os2prfwrc.mk
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

CC_FLAGS=-bt=os2v2 -dDEBUG -dOS2 -d__32BIT__ -d__i386__ -5r -zq -bm -ze -w4 -zld -d2 -hc $(_CC_OPTIONAL) $(CC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CC_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES) -i=$(WATCOM)\h
CC_FLAGS_EXE=$(CC_FLAGS) -zc
CC_FLAGS_DLL=$(CC_FLAGS) -zc -bd
CC_FLAGS_SYS=$(CC_FLAGS) -s -zdp -zff -zgf -zu
CC_FLAGS_VDD=$(CC_FLAGS_SYS)
CC_FLAGS_IFS=$(CC_FLAGS_SYS) -bd
CC_OBJ_OUT=-fo=
CC_LST_OUT=
CC_PC_2_STDOUT=-pc

CXX_FLAGS=-bt=os2v2 -dDEBUG -dOS2 -d__32BIT__ -d__i386__ -5r -zq -bm -ze -w4 -zld -d2 -hc $(_CXX_OPTIONAL)  $(CXX_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CXX_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES) -i=$(WATCOM)\h
CXX_FLAGS_EXE=$(CXX_FLAGS) -zc
CXX_FLAGS_DLL=$(CXX_FLAGS) -zc -bd
CXX_FLAGS_SYS=$(CXX_FLAGS) -s -zdp -zff -zgf -zu
CXX_FLAGS_VDD=$(CXX_FLAGS_SYS)
CXX_FLAGS_IFS=$(CXX_FLAGS_SYS) -bd
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

LINK_FLAGS=Option eliminate, manglednames, caseexact, cache $(_LD_OPTIONAL) Debug codeview all
LINK_FLAGS_EXE=$(LINK_FLAGS)
LINK_FLAGS_DLL=$(LINK_FLAGS)
LINK_FLAGS_SYS=$(LINK_FLAGS) segment type code preload segment type data preload Option internalrelocs, togglerelocs
LINK_FLAGS_VDD=$(LINK_FLAGS_SYS)
LINK_FLAGS_IFS=$(LINK_FLAGS_DLL) segment type code preload segment type data preload
LINK_CMD_EXE=$(LINK) $(LINK_FLAGS_EXE) @$(TARGET_LNK)
LINK_CMD_DLL=$(LINK) $(LINK_FLAGS_DLL) @$(TARGET_LNK)
LINK_CMD_SYS=$(LINK) $(LINK_FLAGS_SYS) @$(TARGET_LNK)
LINK_CMD_VDD=$(LINK) $(LINK_FLAGS_VDD) @$(TARGET_LNK)
LINK_CMD_IFS=$(LINK) $(LINK_FLAGS_IFS) @$(TARGET_LNK)
LINK_LNK1=file       $(TARGET_OBJS: =^
file       )
LINK_LNK2=libpath    $(WATCOM)\lib386\os2;$(WATCOM)\lib386;
LINK_LNK3=option map=$(TARGET_MAP)
LINK_LNK4=library    $(TARGET_LIBS: =^, )
LINK_LNK5=name       $(PATH_TARGET)\$(TARGET_NAME).$(TARGET_EXT)



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

# ---OS2, DEBUG, WAT11-------------------------

