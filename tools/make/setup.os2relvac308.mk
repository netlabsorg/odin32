# $Id: setup.os2relvac308.mk,v 1.1.2.3 2002-04-01 13:57:19 bird Exp $

# ---OS2, RELEASE, VAC308-------------------------
ENV_NAME="OS/2, Release, IBM VisualAge for C++ 3.08"
ENV_STATUS=OK

#
# The tools
#
AR=ilib.exe
AS=alp.exe
CC=icc.exe
CXX=icc.exe
LINK=ilink.exe
IMPLIB=implib.exe
RC=rc.exe
RL=rc.exe
EXEPACK=lxlite.exe


#
# Extentions.
#
EXT_OBJ = obj
EXT_LIB = lib
EXT_ILIB= lib
EXT_EXE = exe
EXT_DLL = dll
EXT_SYS = sys
EXT_RES = res


#
# The flags
#
AR_FLAGS=/nologo /noignorecase
AR_CMD=$(AR) $(AR_FLAGS) $@ @$(TARGET_LNK)
_AR_LNK1= "$(TARGET_OBJS: ="&^
")"
AR_LNK1= $(_AR_LNK1:""=)
AR_LNK2= $(@R).lst

AS_FLAGS    = -Mb -Sv:ALP +Od -D:DEBUG -D:OS2 $(AS_DEFINES:-D=-D:) $(ALL_INCLUDES:-I=-i:) $(AS_INCLUDES:-I=-i:) $(ALL_DEFINES:-D=-D:) -i:$(PATH_INCLUDE)
AS_FLAGS_SYS= $(AS_FLAGS) -D:RING0 -i:$(PATH_DDK)\base\inc
AS_OBJ_OUT= -Fo:

CC_FLAGS=/Q /DOS2 /Ti+ /O+ /G5 /Gf+ /Gi+ /Ss+ /C+ $(CC_SEGS) $(CC_DEFINES) $(ALL_DEFINES) $(CC_INCLUDES) $(ALL_INCLUDES) /I$(PATH_INCLUDE)
CC_FLAGS_EXE=$(CC_FLAGS) /Gm+ /Ge+ /Gn+
CC_FLAGS_DLL=$(CC_FLAGS) /Gm+ /Ge- /Gn-
CC_FLAGS_CRT=$(CC_FLAGS) /Gm+ /Ge-
CC_FLAGS_SYS=$(CC_FLAGS) /Rn  /Ge+ /Gn+ /DRING0 /Gr+ /Gs- -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext-
CC_OBJ_OUT=/Fo
CC_PC_2_STDOUT=/Pd+ /P+

CXX_FLAGS=/Q /DOS2 /Ti+ /O+ /G5 /Gf+ /Gi+ /Ss+ /C+ $(CXX_SEGS) $(CXX_DEFINES) $(ALL_DEFINES) $(CXX_INCLUDES) $(ALL_INCLUDES) /I$(PATH_INCLUDE)
CXX_FLAGS_EXE=$(CXX_FLAGS) /Gm+ /Ge+ /Gn+
CXX_FLAGS_DLL=$(CXX_FLAGS) /Gm+ /Ge- /Gn-
CXX_FLAGS_CRT=$(CXX_FLAGS) /Gm+ /Ge-
CXX_FLAGS_SYS=$(CXX_FLAGS) /Rn  /Ge+ /Gn+ /DRING0 /Gr+ /Gs- /Gx+ /Tm- -Wall+ppt-ppc-inl-cnv-gnr-vft-
CXX_OBJ_OUT=/Fo
CXX_PC_2_STDOUT=/Pd+ /P+

IMPLIB_FLAGS=/NOI /Nologo

LINK_FLAGS=/nofree /nologo /map /NOE /NOD /PACKCODE /PACKDATA /EXEPACK:2 /LINENUMBERS /NODEBUG
LINK_FLAGS_EXE=$(LINK_FLAGS) /EXECutable /BASE:0x10000 /STACK:$(TARGET_STACKSIZE)
LINK_FLAGS_DLL=$(LINK_FLAGS) /DLL
LINK_FLAGS_SYS=$(LINK_FLAGS) /PDD /Align:16 /NOIgnorecase
LINK_CMD_EXE=$(LINK) $(LINK_FLAGS_EXE) @$(TARGET_LNK)
LINK_CMD_DLL=$(LINK) $(LINK_FLAGS_DLL) @$(TARGET_LNK)
LINK_CMD_SYS=$(LINK) $(LINK_FLAGS_SYS) @$(TARGET_LNK)
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

RC_FLAGS=-r -n -i $(PATH_INCLUDE:;= -i ) $(RC_DEFINES) $(RC_INCLUDES)
RL_FLAGS=-x2 -n

#
# Libraries and object files.
#
LIB_OS=os2386.lib
LIB_C_OBJ=cppom30.lib
LIB_C_DLL=cppom30i.lib
LIB_C_RTDLL=cppom30o.lib
OBJ_PROFILE=

# ---OS2, RELEASE, VAC308-------------------------

