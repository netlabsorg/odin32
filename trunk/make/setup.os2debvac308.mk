# $Id: setup.os2debvac308.mk,v 1.14 2002-08-24 04:59:47 bird Exp $

# ---OS2, DEBUG, VAC308-------------------------
ENV_NAME="OS/2, Debug, IBM VisualAge for C++ 3.08"
ENV_STATUS=OK
!if "$(ENV_ENVS)" == ""
ENV_ENVS=vac308
!else
ENV_ENVS_FORCE=vac308
!endif
!undef ENV_16BIT


#
# Include some shared standard stuff: ALP, WRC, VAC optional stuff.
#
!include $(PATH_MAKE)\setup.os2debalp.mk
!include $(PATH_MAKE)\setup.os2debwrc.mk
!include $(PATH_MAKE)\setup.optional.vac3xx.mk


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
AR_FLAGS=/nologo /noignorecase
AR_CMD=$(AR) $(AR_FLAGS) @"$(TARGET_LNK)"
AR_LNK1= "$(@R).$(EXT_LIB)"
AR_LNK2=y
_AR_LNK3= +"$(TARGET_OBJS: ="&^
 +")"
AR_LNK3= $(_AR_LNK3:+""&^
=)
AR_LNK4= "$(@R).lst";

CC_FLAGS=/Q /DDEBUG /DOS2 /D__32BIT__ /D__i386__ /DMODEL=FLAT /Ti+ /O- /Ss+ /C+ $(_CC_OPTIONAL) $(CC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CC_INCLUDES) $(ALL_INCLUDES) /I$(PATH_INCLUDES)
CC_FLAGS_EXE=$(CC_FLAGS) /Gm+ /Ge+
CC_FLAGS_DLL=$(CC_FLAGS) /Gm+ /Ge-
CC_FLAGS_SYS=$(CC_FLAGS) /Rn  /Ge+ /DRING0 /Gr+ /Gs- /Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext-
CC_FLAGS_VDD=$(CC_FLAGS_SYS)
CC_FLAGS_IFS=$(CC_FLAGS_SYS)
CC_OBJ_OUT=/Fo
CC_LST_OUT=/Fa
CC_PC_2_STDOUT=/Pd+ /P+

CXX_FLAGS=/Q /DDEBUG /DOS2 /D__i386__ /DMODEL=FLAT /Ti+ /O- /Ss+ /C+ $(_CXX_OPTIONAL) $(CXX_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CXX_INCLUDES) $(ALL_INCLUDES) /I$(PATH_INCLUDES)
CXX_FLAGS_EXE=$(CXX_FLAGS) /Gm+ /Ge+
CXX_FLAGS_DLL=$(CXX_FLAGS) /Gm+ /Ge-
CXX_FLAGS_SYS=$(CXX_FLAGS) /Rn  /Ge+ /DRING0 /Gr+ /Gs- /Wall+ppt-ppc-inl-cnv-gnr-vft- /Gx+
CXX_FLAGS_VDD=$(CXX_FLAGS_SYS)
CXX_FLAGS_IFS=$(CXX_FLAGS_SYS)
CXX_OBJ_OUT=/Fo
CXX_LST_OUT=/Fa
CXX_PC_2_STDOUT=/Pd+ /P+

!if "$(CC_AS_CXX)" != ""
! if "$(CXX_AS_CC)" != ""
!  if [@(ECHO) $(CLRERR)Error: Do you think you're smart? CC_AS_CXX and CXX_AS_CC is mutual execlusive!$(CLRRST)]
!  endif
!  error
! endif
CC=$(CXX)
CC_FLAGS_EXE=$(CXX_FLAGS_EXE) /Tdp
CC_FLAGS_DLL=$(CXX_FLAGS_DLL) /Tdp
CC_FLAGS_SYS=$(CXX_FLAGS_SYS) /Tdp
CC_FLAGS_VDD=$(CXX_FLAGS_VDD) /Tdp
CC_FLAGS_IFS=$(CXX_FLAGS_IFS) /Tdp
!endif
!if "$(CXX_AS_CC)" != ""
CXX=$(CC)
CXX_FLAGS_EXE=$(CC_FLAGS_EXE) /Tdc
CXX_FLAGS_DLL=$(CC_FLAGS_DLL) /Tdc
CXX_FLAGS_SYS=$(CC_FLAGS_SYS) /Tdc
CXX_FLAGS_VDD=$(CC_FLAGS_VDD) /Tdc
CXX_FLAGS_IFS=$(CC_FLAGS_IFS) /Tdc
!endif

IMPLIB_FLAGS=/NOI /Nologo

LINK_FLAGS=/nofree /nologo /de /map /linenumbers /NOIgnorecase /NOE /NOD /PACKCODE /PACKDATA
LINK_FLAGS_EXE=$(LINK_FLAGS) /EXECutable /STACK:$(TARGET_STACKSIZE)
LINK_FLAGS_DLL=$(LINK_FLAGS) /DLL
LINK_FLAGS_SYS=$(LINK_FLAGS) /PDD /Align:16
LINK_FLAGS_VDD=$(LINK_FLAGS) /VDD /Align:16
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
LINK_LNK5=$(TARGET_DEF_LINK)

RC_FLAGS=-r -n $(RC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(RC_INCLUDES:-I=-i ) $(ALL_INCLUDES:-I=-i ) -i $(PATH_INCLUDES:;= -i )
RL_FLAGS=-x2 -n


#
# Libraries and object files.
#
LIB_OS      = os2386.lib
LIB_C_OBJ   = cppom30.lib
LIB_C_DLL   = cppom30i.lib
LIB_C_RTDLL = cppom30o.lib
LIB_C_NRE   = cppon30.lib
LIB_C_DMNGL = demangl.lib
OBJ_PROFILE =

# ---OS2, DEBUG, VAC308-------------------------

