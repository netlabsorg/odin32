# $Id: setup.os2debvac365.mk,v 1.4 2002-04-22 00:30:10 bird Exp $

# ---OS2, DEBUG, VAC365-------------------------
ENV_NAME="OS/2, Debug, IBM VisualAge for C++ 3.6.5"
ENV_STATUS=OK

#
# Include some shared standard stuff: ALP.
#
!include $(PATH_MAKE)\setup.os2debalp.mk

#
# The tools
#
AR=ilib.exe
CC=icc.exe
CXX=icc.exe
LINK=ilink.exe
IMPLIB=implib.exe
RC=rc.exe
RL=rc.exe
EXEPACK=lxlite.exe

#
# The flags
#
AR_FLAGS=/nofree /nologo /noignorecase
AR_CMD=$(AR) $(AR_FLAGS) $@ @$(TARGET_LNK)
_AR_LNK1= "$(TARGET_OBJS: ="&^
")"
AR_LNK1= $(_AR_LNK1:""=)
AR_LNK2= $(@R).lst

CC_FLAGS=/Q /DDEBUG /DOS2 /Ti+ /O- /Ss+ /C+ $(CC_SEGS) $(CC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CC_INCLUDES) $(ALL_INCLUDES) /I$(PATH_INCLUDES)
CC_FLAGS_EXE=$(CC_FLAGS) /Gm+ /Ge+ /Gn+
CC_FLAGS_DLL=$(CC_FLAGS) /Gm+ /Ge- /Gn-
CC_FLAGS_CRT=$(CC_FLAGS) /Gm+ /Ge-
CC_FLAGS_SYS=$(CC_FLAGS) /Rn  /Ge+ /Gn+ /DRING0 /Gr+ /Gs- -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext-
CC_FLAGS_VDD=$(CC_FLAGS_SYS)
CC_FLAGS_IFS=$(CC_FLAGS_SYS)
CC_OBJ_OUT=/Fo
CC_LST_OUT=/Fa
CC_PC_2_STDOUT=/Pd+ /P+

CXX_FLAGS=/Q /DDEBUG /DOS2 /Ti+ /O- /Ss+ /C+ $(CXX_SEGS) $(CXX_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CXX_INCLUDES) $(ALL_INCLUDES) /I$(PATH_INCLUDES)
CXX_FLAGS_EXE=$(CXX_FLAGS) /Gm+ /Ge+ /Gn+
CXX_FLAGS_DLL=$(CXX_FLAGS) /Gm+ /Ge- /Gn-
CXX_FLAGS_CRT=$(CXX_FLAGS) /Gm+ /Ge-
CXX_FLAGS_SYS=$(CXX_FLAGS) /Rn  /Ge+ /Gn+ /DRING0 /Gr+ /Gs- /Gx+ /Tm- -Wall+ppt-ppc-inl-cnv-gnr-vft-
CXX_FLAGS_VDD=$(CXX_FLAGS_SYS)
CXX_FLAGS_IFS=$(CXX_FLAGS_SYS)
CXX_OBJ_OUT=/Fo
CXX_LST_OUT=/Fa
CXX_PC_2_STDOUT=/Pd+ /P+

IMPLIB_FLAGS=/NOI /Nologo

LINK_FLAGS=/nofree /nologo /de /map /NOE /NOD /Optfunc /PACKCODE /PACKDATA
LINK_FLAGS_EXE=$(LINK_FLAGS) /EXECutable /STACK:$(TARGET_STACKSIZE)
LINK_FLAGS_DLL=$(LINK_FLAGS) /DLL
LINK_FLAGS_SYS=$(LINK_FLAGS) /PDD /Align:16 /NOIgnorecase
LINK_FLAGS_VDD=$(LINK_FLAGS) /VDD /Align:16 /NOIgnorecase
LINK_FLAGS_IFS=$(LINK_FLAGS_DLL)
LINK_CMD_EXE=$(LINK) $(LINK_FLAGS_EXE) @$(TARGET_LNK)
LINK_CMD_DLL=$(LINK) $(LINK_FLAGS_DLL) @$(TARGET_LNK)
LINK_CMD_SYS=$(LINK) $(LINK_FLAGS_SYS) @$(TARGET_LNK)
LINK_CMD_VDD=$(LINK) $(LINK_FLAGS_VDD) @$(TARGET_LNK)
LINK_CMD_IFS=$(LINK) $(LINK_FLAGS_IFS) @$(TARGET_LNK)
LINK_LNK1=$(TARGET_OBJS: =+^
),
LINK_LNK2=$(TARGET),
LINK_LNK3=$(TARGET_MAP),
LINK_LNK4=$(TARGET_LIBS: =+^
),
LINK_LNK5=$(TARGET_DEF)

RC_FLAGS=-r -n -i $(PATH_INCLUDES:;= -i ) $(RC_DEFINES) $(RC_INCLUDES)
RL_FLAGS=-x2 -n

#
# Libraries and object files.
#
LIB_OS      = os2386.lib
LIB_C_OBJ   = cpprms36.lib
LIB_C_DLL   = cpprmi36.lib
LIB_C_RTDLL = cpprmo36.lib
LIB_C_NRE   = cpprni36.lib
LIB_C_DMNGL = cpprds36.lib
OBJ_PROFILE =

# ---OS2, DEBUG, VAC365-------------------------

