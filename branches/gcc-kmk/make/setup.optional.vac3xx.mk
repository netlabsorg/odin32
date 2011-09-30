# $Id: setup.optional.vac3xx.mk,v 1.6 2002-09-14 23:31:27 bird Exp $

#
#  Helper file for all the optional stuff which is common for
#  all vac3x compilers.
#  Concidering the size of this, it really make sense putting it here.
#
MAKE_INCLUDE_CX_OPT = $(PATH_MAKE)\setup.optional.vac3xx.mk


#
# C Compiler flags.
#
_CC_SEG_TEXT    =
_CC_SEG_DATA    =
_CC_SEG_XCPT    =
_CC_DEFAULT_LIBS= /Gn+
_CC_PACK        =
_CC_MODEL       =
!if "$(BUILD_MODE)" == "DEBUG"
_CC_DBGMEM      = /Tm+
!else
_CC_DBGMEM      = /Tm-
!endif
_CC_RING0       =
_CC_RUNTIME     =
_CC_CALLING     =

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
_CC_DEFAULT_LIBS = /Gn-
!endif
!ifdef ALL_PACK
_CC_PACK        = /Sp$(ALL_PACK)
!endif
!ifdef CC_PACK
_CC_PACK        = /Sp$(CC_PACK)
!endif
!if defined(CC_MODEL) || defined(ALL_MODEL)
! if [$(ECHO) warning: CC_MODEL/ALL_MODEL isn't supported by this compiler$(CLRRST)]
! endif
!endif
!if defined(CC_DBGMEM) || defined(ALL_DBGMEM)
_CC_DBGMEM      = /Tm+
!endif
!if defined(CC_NO_DBGMEM) || defined(ALL_NO_DBGMEM)
_CC_DBGMEM      = /Tm-
!endif
!if defined(CC_RING0) || defined(ALL_RING0)
_CC_RING0       = /Gr+
!endif
!if defined(CC_NO_RING0) || defined(ALL_NO_RING0)
_CC_RING0       = /Gr-
!endif
!if defined(CC_CRT) || defined(ALL_CRT)
_CC_RUNTIME     = /Re
!endif
!if defined(CC_NO_CRT) || defined(ALL_NO_CRT)
_CC_RUNTIME     = /Rn
!endif
!if defined(CC_CALLING_SYSTEM) || defined(ALL_CALLING_SYSTEM)
_CC_CALLING     = /Ms+
!endif
!if defined(CC_CALLING_CDECL) || defined(ALL_CALLING_CDECL)
_CC_CALLING     = /Mc+
!endif
!if defined(CC_CALLING_STDCALL) || defined(ALL_CALLING_STDCALL)
_CC_CALLING     = /Mt+
!endif
!if defined(CC_CALLING_OPTLINK) || defined(ALL_CALLING_OPTLINK)
_CC_CALLING     = /Mp+
!endif


_CC_OPTIONAL = $(_CC_SEG_TEXT) $(_CC_SEG_DATA) $(_CC_SEG_XCPT) $(_CC_DEFAULT_LIBS) $(_CC_PACK) $(_CC_MODEL) $(_CC_DBGMEM) $(_CC_RING0) $(_CC_RUNTIME)



#
# C++ Compiler flags.
#
_CXX_SEG_TEXT   =
_CXX_SEG_DATA   =
_CXX_SEG_XCPT   =
_CXX_DEFAULT_LIBS = /Gn+
_CXX_PACK       =
_CXX_XCPT       = /Gx+
_CXX_MODEL      =
!if "$(BUILD_MODE)" == "DEBUG"
_CXX_DBGMEM     = /Tm+
!else
_CXX_DBGMEM     = /Tm-
!endif
_CXX_RING0      =
_CXX_RUNTIME    =
_CXX_CALLING    =

!ifdef ALL_SEG_TEXT
_CXX_SEG_TEXT=/NT$(ALL_SEG_TEXT)
!endif
!ifdef CXX_SEG_TEXT
_CXX_SEG_TEXT=/NT$(CXX_SEG_TEXT)
!endif
!ifdef ALL_SEG_DATA
_CXX_SEG_DATA=/ND$(ALL_SEG_TEXT)
!endif
!ifdef CXX_SEG_DATA
_CXX_SEG_DATA=/ND$(CXX_SEG_TEXT)
!endif
!ifdef ALL_SEG_XCPT
_CXX_SEG_XCPT=/NX$(ALL_SEG_XCPT)
!endif
!ifdef CXX_SEG_XCPT
_CXX_SEG_XCPT=/NX$(CXX_SEG_TEXT)
!endif
!if defined(CXX_DEFAULT_LIBS) || defined(ALL_DEFAULT_LIBS)
_CXX_DEFAULT_LIBS = /Gn-
!endif
!ifdef ALL_PACK
_CXX_PACK       = /Sp$(ALL_PACK)
!endif
!ifdef CXX_PACK
_CXX_PACK       = /Sp$(CXX_PACK)
!endif
!ifdef CXX_XCPT
_CXX_XCPT       = /Gx-
!endif
!ifdef CXX_NO_XCPT
_CXX_XCPT       = /Gx+
!endif
!if defined(CXX_MODEL) || defined(ALL_MODEL)
! if [$(ECHO) warning: CXX_MODEL/ALL_MODEL isn't supported by this compiler$(CLRRST)]
! endif
!endif
!if defined(CXX_DBGMEM) || defined(ALL_DBGMEM)
_CXX_DBGMEM     = /Tm+
!endif
!if defined(CXX_NO_DBGMEM) || defined(ALL_NO_DBGMEM)
_CXX_DBGMEM     = /Tm-
!endif
!if defined(CXX_RING0) || defined(ALL_RING0)
_CXX_RING0      = /Gr+
!endif
!if defined(CXX_NO_RING0) || defined(ALL_NO_RING0)
_CXX_RING0      = /Gr-
!endif
!if defined(CXX_CRT) || defined(ALL_CRT)
_CXX_RUNTIME    = /Re
!endif
!if defined(CXX_NO_CRT) || defined(ALL_NO_CRT)
_CXX_RUNTIME    = /Rn
!endif
!if defined(CXX_CALLING_SYSTEM) || defined(ALL_CALLING_SYSTEM)
_CXX_CALLING    = /Ms+
!endif
!if defined(CXX_CALLING_CDECL) || defined(ALL_CALLING_CDECL)
_CXX_CALLING    = /Mc+
!endif
!if defined(CXX_CALLING_STDCALL) || defined(ALL_CALLING_STDCALL)
_CXX_CALLING    = /Mt+
!endif
!if defined(CXX_CALLING_OPTLINK) || defined(ALL_CALLING_OPTLINK)
_CXX_CALLING    = /Mp+
!endif

_CXX_OPTIONAL = $(_CXX_SEG_TEXT) $(_CXX_SEG_DATA) $(_CXX_SEG_XCPT) $(_CXX_DEFAULT_LIBS) $(_CXX_PACK) $(_CXX_XCPT) $(_CXX_MODEL) $(_CXX_DBGMEM) $(_CXX_RING0) $(_CXX_RUNTIME) $(_CXX_CALLING)

