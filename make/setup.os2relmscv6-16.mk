# $Id: setup.os2relmscv6-16.mk,v 1.9 2002-08-27 03:58:02 bird Exp $

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
!include $(PATH_MAKE)\setup.os2relalp.mk
!include $(PATH_MAKE)\setup.os2relrc.mk
!include $(PATH_MAKE)\setup.os2relwrc.mk
!include $(PATH_MAKE)\setup.optional.mscvx-16.mk


#
# The tools
#
AR=ilib.exe
CC=cl.exe
CXX=false
LINK=ilink.exe
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

CC_FLAGS=/nologo /c /DOS2 /D__16BIT__ /W0 /G2 /Ogeitln /Zi $(_CC_OPTIONAL) $(CC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CC_INCLUDES) $(ALL_INCLUDES) /I$(PATH_INCLUDES)
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

IMPLIB_FLAGS=/NOI /Nologo

LINK_FLAGS=/nofree /nologo /map /linenumbers /NOIgnorecase /NOE /NOD /PACKCODE /PACKDATA /EXEPACK:2 /Debug
LINK_FLAGS_EXE=$(LINK_FLAGS) /EXECutable /BASE:0x10000 /STACK:$(TARGET_STACKSIZE)
LINK_FLAGS_DLL=$(LINK_FLAGS) /DLL
LINK_FLAGS_SYS=$(LINK_FLAGS) /PDD /Align:16
LINK_FLAGS_VDD=$(LINK_FLAGS) /VDD /Align:16
LINK_FLAGS_IFS=$(LINK_FLAGS_DLL)
LINK_CMD_EXE=$(LINK) $(LINK_FLAGS_EXE) @"$(TARGET_LNK)"
LINK_CMD_DLL=$(LINK) $(LINK_FLAGS_DLL) @"$(TARGET_LNK)"
LINK_CMD_SYS=$(LINK) $(LINK_FLAGS_SYS) @"$(TARGET_LNK)"
LINK_CMD_VDD=$(LINK) $(LINK_FLAGS_VDD) @"$(TARGET_LNK)"
LINK_CMD_IFS=$(LINK) $(LINK_FLAGS_IFS) @"$(TARGET_LNK)"
LINK_LNK1=$(TARGET_OBJS: =+^
),
LINK_LNK2=$(TARGET),
LINK_LNK3=$(TARGET_MAP),
LINK_LNK4=$(TARGET_LIBS: =+^
),
LINK_LNK5=$(TARGET_DEF_LINK)



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

