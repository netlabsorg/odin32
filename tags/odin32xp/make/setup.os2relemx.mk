# $Id: setup.os2relemx.mk,v 1.12 2002-09-20 04:10:05 bird Exp $

# ---OS2, DEBUG, EMX----------------------------
ENV_NAME="OS/2, Debug, EMX/GCC Compiler"
ENV_STATUS=OK
!if "$(ENV_ENVS)" == ""
ENV_ENVS=vac308 emx emxpgcc
!else
ENV_ENVS_FORCE=vac308 emx emxpgcc
!endif
!undef ENV_16BIT
SPACE=_


#
# Make sure ilink finds the libraries.
#
!if [set LIB=$(PATH_EMX)\lib\mt;$(LIBRARY_PATH);$(LIB)]
!endif


#
# Include some shared standard stuff: ALP, VAC optional stuff.
#
!include $(PATH_MAKE)\setup.os2as.mk
!include $(PATH_MAKE)\setup.os2allrc.mk
!include $(PATH_MAKE)\setup.os2allwrc.mk
_LD_LAST_LIB = end.$(EXT_LIB)
!include $(PATH_MAKE)\setup.os2ld.mk
!include $(PATH_MAKE)\setup.os2ar.mk
!include $(PATH_MAKE)\setup.os2allipfc.mk
!include $(PATH_MAKE)\setup.optional.emx.mk


#
# The tools
#
CC=gcc.exe
CXX=gcc.exe


#
# The flags
#
CC_FLAGS=-c -DOS2 -D__32BIT__ -DMODEL=FLAT -Zomf -Wall -Wstrict-prototypes -Wno-trigraphs \
         -fno-common -march=i586 -g -O2 $(_CC_OPTIONAL) \
         $(CC_DEFINES:/D=-D) $(ALL_DEFINES:/D=-D) $(BUILD_DEFINES:/D=-D) $(CC_INCLUDES:^;= -I) $(ALL_INCLUDES:^;= -I) -I$(PATH_INCLUDES:^;= -I)
CC_FLAGS_EXE=$(CC_FLAGS:  = ) -Zmt -Zexe
CC_FLAGS_DLL=$(CC_FLAGS:  = ) -Zmt -Zdll
CC_FLAGS_SYS=$(CC_FLAGS:  = ) -DRING0 -mno-probe -Zdll -Zno-rte
CC_FLAGS_VDD=$(CC_FLAGS_SYS:  = )
CC_FLAGS_IFS=$(CC_FLAGS_SYS:  = ) -Zdll
CC_OBJ_OUT=-o$(SPACE:_= )
#CC_LST_OUT=-S -o$(SPACE:_= )
CC_PC_2_STDOUT=-C -E
CC_PC_2_STRIPPED_STDOUT=-E -P
CC_AS_2_FILE=-S -o$(SPACE:_= )


CXX_FLAGS=-c -DOS2 -D__32BIT__ -DMODEL=FLAT -Zomf -Wall -Wstrict-prototypes -Wno-trigraphs \
         -fno-common -march=i586 -g -O2 $(_CXX_OPTIONAL) \
         $(CXX_DEFINES:/D=-D) $(ALL_DEFINES:/D=-D) $(BUILD_DEFINES:/D=-D) $(CXX_INCLUDES:^;= -I) $(ALL_INCLUDES:^;= -I) -I$(PATH_INCLUDES:^;= -I)
CXX_FLAGS_EXE=$(CXX_FLAGS:  = ) -Zmt -Zexe
CXX_FLAGS_DLL=$(CXX_FLAGS:  = ) -Zmt -Zdll
CXX_FLAGS_SYS=$(CXX_FLAGS:  = ) -DRING0 -mno-probe -Zno-rte -fno-exceptions
CXX_FLAGS_VDD=$(CXX_FLAGS_SYS:  = )
CXX_FLAGS_IFS=$(CXX_FLAGS_SYS:  = ) -Zdll
CXX_OBJ_OUT=-o$(SPACE:_= )
#CXX_LST_OUT=-S -o$(SPACE:_= )
CXX_PC_2_STDOUT=-C -E
CXX_PC_2_STRIPPED_STDOUT=-E -P
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

