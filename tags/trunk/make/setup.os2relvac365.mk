# $Id: setup.os2relvac365.mk,v 1.22 2003-06-30 13:28:59 bird Exp $

# ---OS2, RELEASE, VAC365-------------------------
ENV_NAME="OS/2, Release, IBM VisualAge for C++ 3.6.5"
ENV_STATUS=OK
!if "$(ENV_ENVS)" == ""
ENV_ENVS=vac365
!else
ENV_ENVS_FORCE=vac365
!endif
!undef ENV_16BIT


#
# Include some shared standard stuff: ALP, VAC optional stuff.
#
!include $(PATH_MAKE)\setup.os2as.mk
!include $(PATH_MAKE)\setup.os2allrc.mk
!include $(PATH_MAKE)\setup.os2allwrc.mk
!include $(PATH_MAKE)\setup.os2ld.mk
_AR_NOFREE      = 1
!include $(PATH_MAKE)\setup.os2ar.mk
!include $(PATH_MAKE)\setup.os2allipfc.mk
!include $(PATH_MAKE)\setup.optional.vac3xx.mk


#
# The tools
#
CC=icc.exe
CXX=icc.exe


#
# The flags
#
CC_FLAGS=/Q /DOS2 /D__32BIT__ /D__i386__ /DMODEL=FLAT /Ti+ /O+ /G5 /qtune=pentium2 /Ss+ /C+ $(_CC_OPTIONAL) $(CC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CC_INCLUDES) $(ALL_INCLUDES) /I$(PATH_INCLUDES)
CC_FLAGS_EXE=$(CC_FLAGS) /Gm+ /Ge+
CC_FLAGS_DLL=$(CC_FLAGS) /Gm+ /Ge-
CC_FLAGS_SYS=$(CC_FLAGS) /Tm- /Rn  /Ge+ /DRING0 /Gr+ /Gs+ /Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext-
CC_FLAGS_VDD=$(CC_FLAGS_SYS)
CC_FLAGS_IFS=$(CC_FLAGS_SYS) /Ge-
CC_OBJ_OUT=/Fo
CC_LST_OUT=/Fa
CC_PC_2_STDOUT=/Pd+ /P+ /Pc+
CC_PC_2_STRIPPED_STDOUT=/Pd+ /P+ /Pc- /Pe+

CXX_FLAGS=/Q /DOS2 /D__i386__ /DMODEL=FLAT /Ti+ /O+ /G5 /qtune=pentium2 /Ss+ /C+ $(_CXX_OPTIONAL) $(CXX_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CXX_INCLUDES) $(ALL_INCLUDES) /I$(PATH_INCLUDES)
CXX_FLAGS_EXE=$(CXX_FLAGS) /Gm+ /Ge+
CXX_FLAGS_DLL=$(CXX_FLAGS) /Gm+ /Ge-
CXX_FLAGS_SYS=$(CXX_FLAGS) /Tm- /Rn  /Ge+ /DRING0 /Gr+ /Gs+ /Wall+ppt-ppc-inl-cnv-gnr-vft- /Gx+
CXX_FLAGS_VDD=$(CXX_FLAGS_SYS)
CXX_FLAGS_IFS=$(CXX_FLAGS_SYS) /Ge-
CXX_OBJ_OUT=/Fo
CXX_LST_OUT=/Fa
CXX_PC_2_STDOUT=/Pd+ /P+ /Pc+
CXX_PC_2_STRIPPED_STDOUT=/Pd+ /P+ /Pc- /Pe+

!if "$(CC_AS_CXX)" != ""
! if "$(CXX_AS_CC)" != ""
!  if [@(ECHO) $(CLRERR)Error: CC_AS_CXX and CXX_AS_CC is mutual execlusive!$(CLRRST)]
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

# ---OS2, RELEASE, VAC365-------------------------

