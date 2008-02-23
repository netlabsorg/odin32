# $Id: setup.os2relmscv6.mk,v 1.17 2002-09-20 04:10:05 bird Exp $

# ---OS2, RELEASE, MSCV6-------------------------
ENV_NAME="OS/2, Release, Microsoft C v6.0a 32-bit"
ENV_STATUS=OK
!if "$(ENV_ENVS)" == ""
ENV_ENVS=vac308 mscv6
!else
ENV_ENVS_FORCE=vac308 mscv6
!endif
!undef ENV_16BIT


#
# Include some shared standard stuff: ALP.
#
!include $(PATH_MAKE)\setup.os2as.mk
!include $(PATH_MAKE)\setup.os2allrc.mk
!include $(PATH_MAKE)\setup.os2allwrc.mk
!include $(PATH_MAKE)\setup.os2ld.mk
!include $(PATH_MAKE)\setup.os2ar.mk
!include $(PATH_MAKE)\setup.os2allipfc.mk


#
# The tools
#
CC=cl386.exe
CXX=false


#
# The flags
#

#
# C Compiler flags.
#
_CC_SEG_TEXT     =
_CC_SEG_DATA     =
_CC_SEG_XCPT     =
_CC_DEFAULT_LIBS = /Zl
_CC_PACK         = /Zp
_CC_MODEL        =

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
!if defined(CXX_MODEL) || defined(ALL_MODEL)
! if [$(ECHO) warning: CXX_MODEL/ALL_MODEL isn't supported by this compiler$(CLRRST)]
! endif
!endif

_CC_OPTIONAL = $(_CC_SEG_TEXT) $(_CC_SEG_DATA) $(_CC_SEG_XCPT) $(_CC_DEFAULT_LIBS) $(_CC_PACK) $(_CC_MODEL)

CC_FLAGS=/nologo /c /DDEBUG /DOS2 /D__32BIT__ /D__i386__ /W0 /G3 /Ogeitln /Zi $(_CC_OPTIONAL) $(CC_DEFINES) $(ALL_DEFINES) $(BUILD_DEFINES) $(CC_INCLUDES) $(ALL_INCLUDES) /I$(PATH_INCLUDES)
CC_FLAGS_EXE=$(CC_FLAGS)
CC_FLAGS_DLL=$(CC_FLAGS)
CC_FLAGS_SYS=$(CC_FLAGS) /DRING0 /Gs
CC_FLAGS_IFS=$(CC_FLAGS_SYS)
CC_OBJ_OUT=/Fo
CC_LST_OUT=/Fl
CC_PC_2_STDOUT=/P /C /E

CXX_FLAGS=
CXX_FLAGS_EXE=
CXX_FLAGS_DLL=
CXX_FLAGS_SYS=
CXX_FLAGS_IFS=
CXX_OBJ_OUT=
CXX_LST_OUT=
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


#
# Libraries and object files.
#
LIB_OS      = os2386.lib
LIB_C_OBJ   = libc.lib
LIB_C_DLL   = libc.lib
LIB_C_RTDLL = libc.lib
LIB_C_NRE   =
LIB_C_DMNGL =
OBJ_PROFILE =

# ---OS2, RELEASE, MSCV6-------------------------

