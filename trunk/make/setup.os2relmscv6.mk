# $Id: setup.os2relmscv6.mk,v 1.2 2002-04-07 22:43:26 bird Exp $

# ---OS2, RELEASE, MSCV6-------------------------
ENV_NAME="OS/2, Debug, Microsoft C v6.0a"
ENV_STATUS=OK

#
# Include some shared standard stuff: ALP.
#
!include $(PATH_MAKE)\setup.os2relalp.mk

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
AR_CMD=$(AR) $(AR_FLAGS) $@ @$(TARGET_LNK)
_AR_LNK1= "$(TARGET_OBJS: ="&^
")"
AR_LNK1= $(_AR_LNK1:""=)
AR_LNK2= $(@R).lst

CC_FLAGS=-DDEBUG -DOS2 -c -W0 -Asfw $(CC_SEGS) -G2s -Zp -Zl -nologo -Ogeitln -Gs $(CC_DEFINES) $(ALL_DEFINES) $(CC_INCLUDES) $(ALL_INCLUDES) -I$(PATH_INCLUDE)
CC_FLAGS_EXE=$(CC_FLAGS)
CC_FLAGS_DLL=$(CC_FLAGS)
CC_FLAGS_CRT=$(CC_FLAGS)
CC_FLAGS_SYS=$(CC_FLAGS) /DRING0
CC_FLAGS_IFS=$(CC_FLAGS_SYS)
CC_OBJ_OUT=-Fo
CC_PC_2_STDOUT=-P -E

CXX_FLAGS=
CXX_FLAGS_EXE=
CXX_FLAGS_DLL=
CXX_FLAGS_CRT=
CXX_FLAGS_SYS=
CXX_FLAGS_IFS=
CXX_OBJ_OUT=
CXX_PC_2_STDOUT=

IMPLIB_FLAGS=/NOI /Nologo

LINK_FLAGS=/nologo /de /map /NOE /NOD /Optfunc /PACKCODE /PACKDATA
LINK_FLAGS_EXE=$(LINK_FLAGS) /EXECutable /STACK:$(TARGET_STACKSIZE)
LINK_FLAGS_DLL=$(LINK_FLAGS) /DLL
LINK_FLAGS_SYS=$(LINK_FLAGS) /PDD /Align:16 /NOIgnorecase
LINK_FLAGS_IFS=$(LINK_FLAGS_DLL)
LINK_CMD_EXE=$(LINK) $(LINK_FLAGS_EXE) @$(TARGET_LNK)
LINK_CMD_DLL=$(LINK) $(LINK_FLAGS_DLL) @$(TARGET_LNK)
LINK_CMD_SYS=$(LINK) $(LINK_FLAGS_SYS) @$(TARGET_LNK)
LINK_CMD_IFS=$(LINK) $(LINK_FLAGS_IFS) @$(TARGET_LNK)
LINK_LNK1=$(TARGET_OBJS: =^
)
LINK_LNK2=/OUT:$(TARGET)
LINK_LNK3=/MAP:$(TARGET_MAP)
LINK_LNK4=$(TARGET_LIBS: =^
)
LINK_LNK5=$(TARGET_DEF)

RC_FLAGS=-r -n -i $(PATH_INCLUDE:;= -i ) $(RC_DEFINES) $(RC_INCLUDES)
RL_FLAGS=-x2 -n

#
# Libraries and object files.
#
LIB_OS      = os2286.lib
LIB_C_OBJ   = clibcep.lib
LIB_C_DLL   = clibcep.lib
LIB_C_RTDLL = clibcep.lib
LIB_C_NRE   =
LIB_C_DMNGL =
OBJ_PROFILE =

# ---OS2, RELEASE, MSCV6-------------------------


