# $Id: setup.os2relwat11-16.mk,v 1.4 2002-08-20 08:03:00 bird Exp $

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
# Include some shared standard stuff: ALP, VAC optional stuff.
#
!include $(PATH_MAKE)\setup.os2debalp.mk
!include $(PATH_MAKE)\setup.optional.watcom11x.mk


#
# The tools
#
AR=ilib.exe
CC=wcc.exe
CXX=wpp.exe
LINK=wlink.exe
IMPLIB=implib.exe
RC=rc.exe
RL=rc.exe
EXEPACK=lxlite.exe


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
AR_LNK4= "$(@R).lst"

_CC_FLAGS_OS =

# Note: wlink doesn't like 16-bit objects with debuginfo if Debug is disabled. (Failes on make\testcase.)
CC_FLAGS=-bt=os2 -dOS2 -d__16BIT__ -5 -omlinear -zq -bm -ze -w4 -zc $(_CC_OPTIONAL) $(CC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CC_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES) -i=$(WATCOM)\h
CC_FLAGS_EXE=$(CC_FLAGS)
CC_FLAGS_DLL=$(CC_FLAGS) -bd
CC_FLAGS_SYS=$(CC_FLAGS) -s -zfp -zgp -zu
CC_FLAGS_VDD=$(CC_FLAGS_SYS)
CC_FLAGS_IFS=$(CC_FLAGS_SYS) -bd
CC_OBJ_OUT=-fo=
CC_LST_OUT=
CC_PC_2_STDOUT=-pc

# Note: wlink doesn't like 16-bit objects with debuginfo if Debug is disabled. (paranoia since CC failed)
CXX_FLAGS=-bt=os2 -dOS2 -d__16BIT__ -5 -omlinear -zq -bm -ze -w4 -zc $(_CXX_OPTIONAL)  $(CXX_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CXX_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES) -i=$(WATCOM)\h
CXX_FLAGS_EXE=$(CXX_FLAGS)
CXX_FLAGS_DLL=$(CXX_FLAGS) -bd
CXX_FLAGS_SYS=$(CXX_FLAGS) -s -zfp -zgp -zu
CXX_FLAGS_VDD=$(CXX_FLAGS_SYS)
CXX_FLAGS_IFS=$(CXX_FLAGS_SYS) -bd
CXX_OBJ_OUT=-fo=
CXX_LST_OUT=
CXX_PC_2_STDOUT=-pc

!if "$(CC_AS_CXX)" != ""
! if "$(CXX_AS_CC)" != ""
!  if [@(ECHO) $(CLRERR)Error: Do you think you're smart? CC_AS_CXX and CXX_AS_CC is mutual execlusive!$(CLRRST)]
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
CXX_FLAGS_EXE=$(C_FLAGS_EXE)
CXX_FLAGS_DLL=$(C_FLAGS_DLL)
CXX_FLAGS_SYS=$(C_FLAGS_SYS)
CXX_FLAGS_VDD=$(C_FLAGS_VDD)
CXX_FLAGS_IFS=$(C_FLAGS_IFS)
!endif

IMPLIB_FLAGS=/NOI /Nologo

LINK_FLAGS=Sort global Option quiet, dosseg, eliminate, manglednames, caseexact
LINK_FLAGS_EXE=$(LINK_FLAGS)
LINK_FLAGS_DLL=$(LINK_FLAGS)
LINK_FLAGS_SYS=$(LINK_FLAGS) Option oneautodata, internalrelocs, togglerelocs
LINK_FLAGS_VDD=$(LINK_FLAGS_SYS)
LINK_FLAGS_IFS=$(LINK_FLAGS_SYS)
LINK_CMD_EXE=$(LINK) $(LINK_FLAGS_EXE) @$(TARGET_LNK)
LINK_CMD_DLL=$(LINK) $(LINK_FLAGS_DLL) @$(TARGET_LNK)
LINK_CMD_SYS=$(LINK) $(LINK_FLAGS_SYS) @$(TARGET_LNK)
LINK_CMD_VDD=$(LINK) $(LINK_FLAGS_VDD) @$(TARGET_LNK)
LINK_CMD_IFS=$(LINK) $(LINK_FLAGS_IFS) @$(TARGET_LNK)
LINK_LNK1=file       $(TARGET_OBJS: =^
file       )
LINK_LNK2=libpath    $(WATCOM)\lib286\os2;$(WATCOM)\lib286;
LINK_LNK3=option map=$(TARGET_MAP)
LINK_LNK4=library    $(TARGET_LIBS: =^, )
LINK_LNK5=name       $(PATH_TARGET)\$(TARGET_NAME).$(TARGET_EXT)

RC_FLAGS=-r -n $(RC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(RC_INCLUDES:-I=-i ) $(ALL_INCLUDES:-I=-i ) -i $(PATH_INCLUDES:;= -i )
RL_FLAGS=-x2 -n


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

