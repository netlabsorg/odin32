# $Id: setup.os2prfigcc.mk,v 1.1 2004-03-09 15:23:22 bird Exp $

# ---OS2, DEBUG, EMX----------------------------
ENV_NAME="OS/2, Profile, Innotek GCC Compiler"
ENV_STATUS=OK
!if "$(ENV_ENVS)" == ""
ENV_ENVS=vac308 gcc322
!else
ENV_ENVS_FORCE=vac308 gcc322
!endif
SPACE=_
!undef ENV_16BIT


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
!include $(PATH_MAKE)\setup.optional.igcc.mk


#
# The tools
#
CC=gcc.exe
CXX=g++.exe


#
# The flags
#
CC_FLAGS=-c -DDEBUG -DOS2 -D__32BIT__ -DMODEL=FLAT -Zomf -mcpu=i586 -g -pg \
		 -pedantic -Wall -Wconversion -Wpointer-arith -Wcast-align -Woverloaded-virtual -Wsynth -Wno-long-long -Wshadow \
         -Wstrict-prototypes -Wno-trigraphs -Wmissing-prototypes -Wwrite-strings -Winline -Wdisabled-optimization \
		 $(_CC_OPTIONAL) \
         $(CC_DEFINES:/D=-D) $(ALL_DEFINES:/D=-D) $(BUILD_DEFINES:/D=-D) $(CC_INCLUDES:^;= -I) $(ALL_INCLUDES:^;= -I) -I$(PATH_INCLUDES:^;= -I)
CC_FLAGS_EXE=$(CC_FLAGS:  = ) -Zexe
CC_FLAGS_DLL=$(CC_FLAGS:  = ) -Zdll
CC_FLAGS_SYS=$(CC_FLAGS:  = ) -DRING0 -mno-probe
CC_FLAGS_VDD=$(CC_FLAGS_SYS:  = )
CC_FLAGS_IFS=$(CC_FLAGS_SYS:  = ) -Zdll
CC_OBJ_OUT=-o$(SPACE:_= )
#CC_LST_OUT=-S -o$(SPACE:_= )
CC_PC_2_STDOUT=-C -E
CC_PC_2_STRIPPED_STDOUT=-E -P
CC_AS_2_FILE=-S -o$(SPACE:_= )


CXX_FLAGS=-c -DDEBUG -DOS2 -D__32BIT__ -DMODEL=FLAT -Zomf -Wall -mcpu=i586 -g -pg \
		 -pedantic -Wall -Wconversion -Wpointer-arith -Wcast-align -Woverloaded-virtual -Wsynth -Wno-long-long -Wshadow \
         -Wstrict-prototypes -Wno-trigraphs -Wmissing-prototypes -Wwrite-strings -Winline -Wdisabled-optimization \
		 $(_CXX_OPTIONAL) \
         $(CXX_DEFINES:/D=-D) $(ALL_DEFINES:/D=-D) $(BUILD_DEFINES:/D=-D) $(CXX_INCLUDES:^;= -I) $(ALL_INCLUDES:^;= -I) -I$(PATH_INCLUDES:^;= -I)
CXX_FLAGS_EXE=$(CXX_FLAGS:  = ) -Zexe
CXX_FLAGS_DLL=$(CXX_FLAGS:  = ) -Zdll
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
LIB_OS      = $(PATH_IGCC)\lib\libos2.lib
LIB_C_OBJ   = $(PATH_IGCC)\lib\libc_alias.lib $(PATH_IGCC)\lib\libc_dll.lib $(PATH_IGCC)\lib\libstdc++.lib
LIB_C_DLL   = $(PATH_IGCC)\lib\libc_alias.lib $(PATH_IGCC)\lib\libc_dll.lib $(PATH_IGCC)\lib\libstdc++.lib
LIB_C_RTDLL = $(PATH_IGCC)\lib\libc_alias.lib $(PATH_IGCC)\lib\libc_dll.lib $(PATH_IGCC)\lib\libstdc++.lib
LIB_C_NRE   =
LIB_C_DMNGL =
OBJ_PROFILE =

# ---OS2, DEBUG, EMX----------------------------

