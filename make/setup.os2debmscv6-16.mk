# $Id: setup.os2debmscv6-16.mk,v 1.6 2002-08-20 21:16:48 bird Exp $

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
!include $(PATH_MAKE)\setup.os2debalp.mk


#
# The tools
#
AR=ilib.exe
CC=cl.exe
CXX=false
LINK=ilink.exe
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
AR_LNK4= "$(@R).lst";


#
# C Compiler flags.
#
_CC_SEG_TEXT     =
_CC_SEG_DATA     =
_CC_SEG_XCPT     =
_CC_DEFAULT_LIBS = /Zl
_CC_PACK         = /Zp
_CC_MODEL        = /Asfw
_OBJ_MODEL       = c

!ifdef ALL_SEG_TEXT
_CC_SEG_TEXT=/NT$(ALL_SEG_TEXT)
!endif
!ifdef CC_SEG_TEXT
_CC_SEG_TEXT=/NT$(CC_SEG_TEXT)
!endif
!ifdef ALL_SEG_DATA
_CC_SEG_DATA=/ND$(ALL_SEG_TEXT)
!endif
!ifdef CC_SEG_DATA
_CC_SEG_DATA=/ND$(CC_SEG_TEXT)
!endif
!if defined(CC_DEFAULT_LIBS) || defined(ALL_DEFAULT_LIBS)
_CC_DEFAULT_LIBS =
!endif
!ifdef ALL_PACK
_CC_PACK        = /Zp$(ALL_PACK)
!endif
!ifdef CC_PACK
_CC_PACK        = /Zp$(CC_PACK)
!endif
# Model
!if !defined(CC_MODEL) && defined(ALL_MODEL)
CC_MODEL    = $(ALL_MODEL)
!endif
!ifdef CC_MODEL
_CC_MODEL   =
!endif
!if "$(CC_MODEL)" == "TINY"
_CC_MODEL   = /AT
_OBJ_MODEL  = s
!endif
!if "$(CC_MODEL)" == "SMALL"
_CC_MODEL   = /AS
_OBJ_MODEL  = s
!endif
!if "$(CC_MODEL)" == "COMPACT"
_CC_MODEL   = /AC
_OBJ_MODEL  = c
!endif
!if "$(CC_MODEL)" == "MEDIUM"
_CC_MODEL   = /AM
_OBJ_MODEL  = m
!endif
!if "$(CC_MODEL)" == "LARGE"
_CC_MODEL   = /AL
_OBJ_MODEL  = l
!endif
!if "$(CC_MODEL)" == "HUGE"
_CC_MODEL   = /AH
_OBJ_MODEL  = l
!endif
!if "$(_CC_MODEL)" == ""
! error Invalid MODEL. CC_MODEL=$(CC_MODEL)
!endif

_CC_OPTIONAL = $(_CC_SEG_TEXT) $(_CC_SEG_DATA) $(_CC_SEG_XCPT) $(_CC_DEFAULT_LIBS) $(_CC_PACK) $(_CC_MODEL)

CC_FLAGS=/nologo /c /DDEBUG /DOS2 /D__16BIT__ /W0 /G2s /Zi /Owis $(_CC_OPTIONAL) $(CC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CC_INCLUDES) $(ALL_INCLUDES) /I$(PATH_INCLUDES)
CC_FLAGS_EXE=$(CC_FLAGS)
CC_FLAGS_DLL=$(CC_FLAGS)
CC_FLAGS_SYS=$(CC_FLAGS) /DRING0
CC_FLAGS_IFS=$(CC_FLAGS_SYS)
CC_OBJ_OUT=/Fo
CC_LST_OUT=/Fa
CC_PC_2_STDOUT=/P /E

CXX_FLAGS=
CXX_FLAGS_EXE=
CXX_FLAGS_DLL=
CXX_FLAGS_SYS=
CC_FLAGS_IFS=
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

IMPLIB_FLAGS=/NOI /Nologo

LINK_FLAGS=/nofree /nologo /de /map /linenumbers /NOIgnorecase /NOE /NOD /Optfunc /PACKCODE /PACKDATA
LINK_FLAGS_EXE=$(LINK_FLAGS) /EXECutable /STACK:$(TARGET_STACKSIZE)
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

RC_FLAGS=-r -n $(RC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(RC_INCLUDES:-I=-i ) $(ALL_INCLUDES:-I=-i ) -i $(PATH_INCLUDES:;= -i )
RL_FLAGS=-x2 -n


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

