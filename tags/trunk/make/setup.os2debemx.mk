# $Id: setup.os2debemx.mk,v 1.6 2002-08-24 22:33:11 bird Exp $

# ---OS2, DEBUG, EMX----------------------------
ENV_NAME="OS/2, Debug, EMX/GCC Compiler"
ENV_STATUS=OK
!if "$(ENV_ENVS)" == ""
ENV_ENVS=vac308 emx emxpgcc
!else
ENV_ENVS_FORCE=vac308 emx emxpgcc
!endif
SPACE=_
!undef ENV_16BIT


#
# Make sure ilink finds the libraries.
#
!if [set LIB=$(PATH_EMX)\lib\mt;$(LIBRARY_PATH);$(LIB)]
!endif


#
# Include some shared standard stuff: ALP, VAC optional stuff.
#
!include $(PATH_MAKE)\setup.os2debalp.mk
!include $(PATH_MAKE)\setup.os2prfwrc.mk
!include $(PATH_MAKE)\setup.optional.emx.mk


#
# The tools
#
!if 1
AR=ilib.exe
!else
AR=lib.exe
!endif
CC=gcc.exe
CXX=gcc.exe
LINK=ilink.exe
IMPLIB=implib.exe
RC=rc.exe
RL=rc.exe


#
# The flags
#
AR_FLAGS=/nologo /noignorecase
!if 1
AR_CMD=$(AR) $(AR_FLAGS) @"$(TARGET_LNK)"
AR_LNK1= "$(@R).$(EXT_LIB)"
AR_LNK2=y
_AR_LNK3= +"$(TARGET_OBJS: ="&^
 +")"
AR_LNK3= $(_AR_LNK3:+""&^
=)
AR_LNK4= "$(@R).lst";
!else
_AR_LNK1= $(TARGET), ^
+"$(TARGET_OBJS: ="+^
")"
AR_LNK1= $(_AR_LNK1:""=)
AR_LNK2= $(@R).lst
!endif

CC_FLAGS=-c -DDEBUG -DOS2 -D__32BIT__ -DMODEL=FLAT -Zomf -Wall -Wstrict-prototypes -Wno-trigraphs \
         -fno-common -march=i586 -g $(_CC_OPTIONAL) \
         $(CC_DEFINES:/D=-D) $(ALL_DEFINES:/D=-D) $(BUILD_DEFINES:/D=-D) $(CC_INCLUDES:^;= -I) $(ALL_INCLUDES:^;= -I) -I$(PATH_INCLUDES:^;= -I)
CC_FLAGS_EXE=$(CC_FLAGS:  = ) -Zmt -Zexe
CC_FLAGS_DLL=$(CC_FLAGS:  = ) -Zmt -Zdll
CC_FLAGS_SYS=$(CC_FLAGS:  = ) -DRING0 -mno-probe -Zdll -Zno-rte
CC_FLAGS_VDD=$(CC_FLAGS_SYS:  = )
CC_FLAGS_IFS=$(CC_FLAGS_SYS:  = ) -Zdll
CC_OBJ_OUT=-o$(SPACE:_= )
#CC_LST_OUT=-S -o$(SPACE:_= )
CC_PC_2_STDOUT=-C -E
CC_AS_2_FILE=-S -o$(SPACE:_= )


CXX_FLAGS=-c -DDEBUG -DOS2 -D__32BIT__ -DMODEL=FLAT -Zomf -Wall -Wstrict-prototypes -Wno-trigraphs \
         -fno-common -march=i586 -g $(_CXX_OPTIONAL) \
         $(CXX_DEFINES:/D=-D) $(ALL_DEFINES:/D=-D) $(BUILD_DEFINES:/D=-D) $(CXX_INCLUDES:^;= -I) $(ALL_INCLUDES:^;= -I) -I$(PATH_INCLUDES:^;= -I)
CXX_FLAGS_EXE=$(CXX_FLAGS:  = ) -Zmt -Zexe
CXX_FLAGS_DLL=$(CXX_FLAGS:  = ) -Zmt -Zdll
CXX_FLAGS_SYS=$(CXX_FLAGS:  = ) -DRING0 -mno-probe -Zno-rte -fno-exceptions
CXX_FLAGS_VDD=$(CXX_FLAGS_SYS:  = )
CXX_FLAGS_IFS=$(CXX_FLAGS_SYS:  = ) -Zdll
CXX_OBJ_OUT=-o$(SPACE:_= )
#CXX_LST_OUT=-S -o$(SPACE:_= )
CXX_PC_2_STDOUT=-C -E
CXX_AS_2_FILE=-S -o$(SPACE:_= )

!if "$(CC_AS_CXX)" != ""
! if "$(CXX_AS_CC)" != ""
!  if [@(ECHO) $(CLRERR)Error: Do you think you're smart? CC_AS_CXX and CXX_AS_CC is mutual execlusive!$(CLRRST)]
!  endif
!  error
! endif
CC=g++
CC_FLAGS_EXE=$(CXX_FLAGS_EXE)
CC_FLAGS_DLL=$(CXX_FLAGS_DLL)
CC_FLAGS_SYS=$(CXX_FLAGS_SYS)
CC_FLAGS_VDD=$(CXX_FLAGS_VDD)
CC_FLAGS_IFS=$(CXX_FLAGS_IFS)
!endif
!if "$(CXX_AS_CC)" != ""
! if [@(ECHO) $(CLRERR)Error: CXX_AS_CC is not supported by this compiler!$(CLRRST)]
! endif
! error
!endif

IMPLIB_FLAGS=/NOI /Nologo

LINK_FLAGS=/nofree /nologo /de /map /linenumbers /NOIgnorecase /NOE /NOD /PACKCODE /PACKDATA
LINK_FLAGS_EXE=$(LINK_FLAGS) /EXECutable /STACK:$(TARGET_STACKSIZE)
LINK_FLAGS_DLL=$(LINK_FLAGS) /DLL
LINK_FLAGS_SYS=$(LINK_FLAGS) /PDD /Align:16
LINK_FLAGS_VDD=$(LINK_FLAGS) /VDD /Align:16
LINK_FLAGS_IFS=$(LINK_FLAGS_DLL)
LINK_CMD_EXE=$(LINK) $(LINK_FLAGS_EXE) crt0.obj @$(TARGET_LNK)
LINK_CMD_DLL=$(LINK) $(LINK_FLAGS_DLL) dll0.obj @$(TARGET_LNK)
LINK_CMD_SYS=$(LINK) $(LINK_FLAGS_SYS) crt0.obj @$(TARGET_LNK)
LINK_CMD_VDD=$(LINK) $(LINK_FLAGS_VDD) crt0.obj @$(TARGET_LNK)
LINK_CMD_IFS=$(LINK) $(LINK_FLAGS_IFS) dll0.obj @$(TARGET_LNK)
LINK_LNK1=$(TARGET_OBJS: =+^
),
LINK_LNK2=$(TARGET),
LINK_LNK3=$(TARGET_MAP),
LINK_LNK4=$(TARGET_LIBS: =+^
) end.lib,
LINK_LNK5=$(TARGET_DEF_LINK)

RC_FLAGS=-r -n $(RC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(RC_INCLUDES:-I=-i ) $(ALL_INCLUDES:-I=-i ) -i $(PATH_INCLUDES:;= -i )
RL_FLAGS=-x2 -n

#
# Libraries and object files.
#
LIB_OS      = os2386.lib
LIB_C_OBJ   = c_alias.lib gcc.lib g.lib c.lib c_app.lib c.lib gcc.lib emx.lib os2.lib emx2.lib
LIB_C_DLL   = c_alias.lib c_static.lib c_import.lib os2.lib emx2.lib
LIB_C_RTDLL = c_alias.lib c_static.lib emx2.lib
LIB_C_NRE   = c_alias.lib gcc.lib g.lib c.lib c_dllnrt.lib os2.lib
LIB_C_DMNGL =
OBJ_PROFILE =

# ---OS2, DEBUG, EMX----------------------------

