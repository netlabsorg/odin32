# $Id: setup.win32debwat11.mk,v 1.1 2002-04-13 04:39:38 bird Exp $

# ---OS2, DEBUG, VAC308-------------------------
ENV_NAME="OS/2, Debug, IBM VisualAge for C++ 3.08"
ENV_STATUS=OK

#
# Include some shared standard stuff: ALP.
#
!include $(PATH_MAKE)\setup.os2debalp.mk

#
# The tools
#
AR=ilib.exe
CC=wcc386.exe
CXX=wpp386.exe
LINK=wlink.exe
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

CC_FLAGS_OS = -bt=nt

CC_FLAGS=$(CC_FLAGS_OS) -zq -zl -bm -ze -w4 -dDEBUG -d2 -hc -zc $(CC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CC_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES) -i=$(WATCOM)\h
CC_FLAGS_EXE=$(CC_FLAGS)
CC_FLAGS_DLL=$(CC_FLAGS) -bd
CC_FLAGS_CRT=$(CC_FLAGS) -bd -br
CC_FLAGS_SYS=$(CC_FLAGS)
CC_FLAGS_IFS=$(CC_FLAGS_SYS)
CC_OBJ_OUT=-fo=
CC_LST_OUT=
CC_PC_2_STDOUT=

CXX_FLAGS=$(CC_FLAGS_OS) -zq -zl -bm -ze -w4 -dDEBUG -d2 -hc -zc $(CXX_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CXX_INCLUDES:-I=-i=) $(ALL_INCLUDES:-I=-i=) -i=$(PATH_INCLUDES) -i=$(WATCOM)\h
CXX_FLAGS_EXE=$(CXX_FLAGS)
CXX_FLAGS_DLL=$(CXX_FLAGS) -bd
CXX_FLAGS_CRT=$(CXX_FLAGS) -bd -br
CXX_FLAGS_SYS=$(CXX_FLAGS) -s -zdp -zff -zgf
CC_FLAGS_IFS=$(CC_FLAGS_SYS)
CXX_OBJ_OUT=-fo=
CXX_PC_2_STDOUT=

IMPLIB_FLAGS=/NOI /Nologo

LINK_FLAGS=/nofree /nologo /de /map /NOE /NOD /Optfunc /PACKCODE /PACKDATA
LINK_FLAGS_EXE=$(LINK_FLAGS) /EXECutable /STACK:$(TARGET_STACKSIZE)
LINK_FLAGS_DLL=$(LINK_FLAGS) /DLL
LINK_FLAGS_SYS=$(LINK_FLAGS) /PDD /Align:16 /NOIgnorecase
LINK_FLAGS_IFS=$(LINK_FLAGS_DLL)
LINK_CMD_EXE=$(LINK) $(LINK_FLAGS_EXE) @$(TARGET_LNK)
LINK_CMD_DLL=$(LINK) $(LINK_FLAGS_DLL) @$(TARGET_LNK)
LINK_CMD_SYS=$(LINK) $(LINK_FLAGS_SYS) @$(TARGET_LNK)
LINK_CMD_IFS=$(LINK) $(LINK_FLAGS_IFS) @$(TARGET_LNK)
!if 0 # nofree works better for me (kso) when working with my drivers...
LINK_LNK1=$(TARGET_OBJS: =^
)
LINK_LNK2=/OUT:$(TARGET)
LINK_LNK3=/MAP:$(TARGET_MAP)
LINK_LNK4=$(TARGET_LIBS: =^
)
LINK_LNK5=$(TARGET_DEF)
!else
LINK_LNK1=$(TARGET_OBJS: =+^
),
LINK_LNK2=$(TARGET),
LINK_LNK3=$(TARGET_MAP),
LINK_LNK4=$(TARGET_LIBS: =+^
),
LINK_LNK5=$(TARGET_DEF)
!endif

#TODO
LINK_FLAGS=format WIndows NT RUntime CONsole option quiet, map
LINK_FLAGS_EXE=$(LINK_FLAGS)
LINK_FLAGS_DLL=$(LINK_FLAGS)
LINK_FLAGS_SYS=format os2 physdevice option int, dosseg, map, eliminate, mang, tog sort global d codeview
LINK_FLAGS_IFS=$(LINK_FLAGS_DLL)
LINK_CMD_EXE=$(LINK) $(LINK_FLAGS_EXE) @$(TARGET_LNK)
LINK_CMD_DLL=$(LINK) $(LINK_FLAGS_DLL) @$(TARGET_LNK)
LINK_CMD_SYS=$(LINK) $(LINK_FLAGS_SYS) @$(TARGET_LNK)
LINK_CMD_IFS=$(LINK) $(LINK_FLAGS_IFS) @$(TARGET_LNK)
TARGET_OBJS1 = $(TARGET_OBJS:  = )
LINK_LNK1=file $(TARGET_OBJS1: =^
file )
LINK_LNK2=libpath $(WATCOM)\lib386;$(WATCOM)\lib386\nt;
LINK_LNK3=option map=$(TARGET_MAP)
LINK_LNK4=library $(TARGET_LIBS: =^, )
LINK_LNK5=name $(PATH_TARGET)\$(TARGET_NAME).$(TARGET_EXT)

RC_FLAGS=-r -n -i $(PATH_INCLUDES:;= -i ) $(RC_DEFINES) $(RC_INCLUDES)
RL_FLAGS=-x2 -n

#
# Libraries and object files.
#
LIB_OS      = $(WATCOM)\lib386\nt\kernel32.lib $(WATCOM)\lib386\nt\user32.lib $(WATCOM)\lib386\nt\gdi32.lib
LIB_C_OBJ   = clib3r.lib
LIB_C_DLL   = clbrdll.lib
LIB_C_RTDLL = clbrdll.lib
LIB_C_NRE   = clib3r.lib
LIB_C_DMNGL =
OBJ_PROFILE =

# ---OS2, DEBUG, VAC308-------------------------

