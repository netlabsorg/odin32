# $Id: odin32.profile.vac3.mk,v 1.7 2001-11-29 00:24:19 phaller Exp $

#
# Odin32 API
#
#       PROFILE makefile for the VAC3 compiler environment.
#


#
# Library macros.
#
SOMLIB   = somtk.lib
RTLLIB   = cppom30.lib
RTLLIB_O = cppom30o.lib
RTLLIB_NRE = cppon30.lib
DLLENTRY = $(ODIN32_LIB)\dllentry.obj
DEMANGLLIB = demangl.lib
ODINCRT  = odincrtp


#
# Tools
#
CC     = icc
CXX    = icc
AS     = alp
LD     = icc
LD2    = $(EXECEXCL) $(ODIN32_TOOLS)\ilink.lock.lnk ilink
IMPLIB = implib
ILIB   = ilib
RC     = $(ODIN32_TOOLS)\wrc
OS2RC  = rc


#
# Tool Flags
#
ASFLAGS          = -Mb -Sc -Sv:ALP +Od -D:DEBUG
IMPLIBFLAGS      = /nologo
ILIBFLAGS        = /quiet /nologo /noignorecase
RCFLAGS          = -s -I. -I$(CPPMAIN)\include -D__WIN32OS2__
!ifdef WRC_PREFIX_RESOURCE
RCFLAGS          = $(RCFLAGS) -p $(TARGET)
!endif
OS2RCFLAGS       = -r -n
OS2RCLFLAGS      = -x2 -n


#
# C Compiler flags.
#   This may look a bit like a mess but, there is usually a reason behind
#   every line and the way it is done. We have some problems with nmake
#   when we add new flags to for example CXXFLAGS too many times.
#
!ifdef PROFILEHOOKS
PROFILEFLAGS=-Gh+
!endif
!ifdef EXETARGET
CTARGETFLAGS     = -Ge+
CXXTARGETFLAGS   = -Ge+
!else
CTARGETFLAGS     = -Ge-
CXXTARGETFLAGS   = -Ge-
!endif
CXXEXCEPTIONS    = -Gx-
CFLAGS           = -c -Q -Si -Ti -Tm+ -Ss+ -W3 -Gm+ /Gn+ $(PROFILEFLAGS) $(CTARGETFLAGS)
CXXFLAGS         = -c -Q -Si -Ti -Tm+ -Ss+ -W3 -Gm+ /Gn+ $(PROFILEFLAGS) $(CXXTARGETFLAGS) /Gx+
CXXFLAGS_ODINCRT = -c -Q -Si -Ti -Tm+ -Ss+ -W3 -Gm+ /Gn- $(PROFILEFLAGS) $(CXXTARGETFLAGS) /Gx+
CFLAGS_WIN32APP  = -c -Q -Si -Ti -Tm+ -Ss+ -W3 -Gm+ /Gn+ $(PROFILEFLAGS) $(CTARGETFLAGS)
CXXFLAGS_WIN32APP= -c -Q -Si -Ti -Tm+ -Ss+ -W3 -Gm+ /Gn+ $(PROFILEFLAGS) $(CXXTARGETFLAGS)
CINCLUDES        = -I$(ODIN32_INCLUDE)\Win -I. -I$(ODIN32_INCLUDE)
CDEFINES_WIN32APP= -DDEBUG -DPROFILE -D__WIN32OS2__ -D__i386__
CDEFINES_ODINCRT = -DDEBUG -DPROFILE -D__WIN32OS2__ -D__i386__ -D__WINE__
CDEFINES         = -DDEBUG -DPROFILE -D__WIN32OS2__ -D__i386__ -D__WINE__ -DTCPV40HDRS -DCOMCTL32UNDOC \
!ifdef DEBUGALL
                   -DDEBUG_ENABLELOG_LEVEL2 \
!endif
!ifdef NODEBUGINFO
                   -DDEFAULT_LOGGING_OFF
!endif


#
# Linker flags.
#   This may look a bit like a mess but, there is usually a reason behind
#   every line and the way it is done. We have some problems with nmake
#   when we add new flags to for example LD2FLAGS too many times.
#
!ifdef EXETARGET
!ifndef STACKSIZE
STACKSIZE = 0x50000
!endif

!   ifdef VIO
LDTARGETFLAGS    = -Ge+ -B"/pmtype:vio /stack:$(STACKSIZE)"
LD2TARGETFLAGS   = /EXEC /pmtype:vio   /stack:$(STACKSIZE)
!   else
LDTARGETFLAGS    = -Ge+ -B"/pmtype:pm /stack:$(STACKSIZE)"
LD2TARGETFLAGS   = /EXEC /pmtype:pm   /stack:$(STACKSIZE)
!   endif
!else
LDTARGETFLAGS    = -Ge-
LD2TARGETFLAGS   = /DLL
!endif
!ifdef NODEBUGINFO
LDFLAGS          = -Q   -B"/noe /map /packcode /packdata /exepack:2 /nodebug /nodbgpack /nod" -Si -W3 -Gm+ $(LDTARGETFLAGS)
LDFLAGS_ODINCRT  = -Q   -B"/noe /map /packcode /packdata /exepack:2 /nodebug /nodbgpack"      -Si -W3 -Gm+ $(LDTARGETFLAGS)
LD2FLAGS         = /nologo /noe /map /packcode /packdata /exepack:2 /nodebug /nodbgpack /nod $(LD2TARGETFLAGS)
LD2FLAGS_ODINCRT = /nologo /noe /map /packcode /packdata /exepack:2 /nodebug /nodbgpack      $(LD2TARGETFLAGS)
!else
LDFLAGS          = -Q   -B"/noe /map /packcode /packdata /exepack:2 /dbgpack /nod" -Ti -Si -W3 -Gm+ $(LDTARGETFLAGS)
LDFLAGS_ODINCRT  = -Q   -B"/noe /map /packcode /packdata /exepack:2 /dbgpack "     -Ti -Si -W3 -Gm+ $(LDTARGETFLAGS)
LD2FLAGS         = /nologo /noe /map /packcode /packdata /exepack:2 /dbgpack /nod /debug $(LD2TARGETFLAGS)
LD2FLAGS_ODINCRT = /nologo /noe /map /packcode /packdata /exepack:2 /dbgpack      /debug $(LD2TARGETFLAGS)
!endif

