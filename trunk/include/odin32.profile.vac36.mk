# $Id: odin32.profile.vac36.mk,v 1.9 2003-02-28 10:28:11 sandervl Exp $

#
# Odin32 API.
#
#       DEBUG makefile for the VAC 3.6.5 compiler environment.
#


#
# Library macros.
#
SOMLIB   = somtk.lib
RTLLIB   = cpprms36.lib
RTLLIB_O = cpprmo36.lib
RTLLIB_NRE = cpprni36.lib
DLLENTRY = $(ODIN32_LIB)\dllentry.obj
ODINCRT  = odincrt
!ifdef DEBUG
ODINCRT  = odincrtd
!endif
LIB_DEMANGLE = cpprds36.lib
!ifndef DEBUG
#OBJ_PROFILE = $(ODIN32_LIB)\cppopa3.obj $(ODIN32_LIB)\cppopa3_fix2.obj
LIB_PROFILE = $(ODIN32_LIB)\kProfile.lib
NO_LXLITE = 1
!endif


#
# Tools
#
CC     = icc
AS     = alp
CXX    = icc
LD     = icc
LD2    = ilink
IMPLIB = implib
ILIB   = ilib
RC     = $(ODIN32_TOOLS)\wrc
OS2RC  = rc

!ifdef EXETARGET
STATIC_CRT=1
!endif

#
# This knuts take at profiling.
#   - Profile the RELEASE version of the system using a real profiler.
#
!ifndef DEBUG

#
# Tool Flags
#
ASFLAGS          = -Mb -Sc -Sv:ALP +Od
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
!ifndef NOPROFILEHOOKS
PROFILEFLAGS     = -Gh+
!endif
!ifdef EXETARGET
CTARGETFLAGS     = -Ge+
CXXTARGETFLAGS   = -Ge+
!else
CTARGETFLAGS     = -Ge-
CXXTARGETFLAGS   = -Ge-
!endif
CXXEXCEPTIONS    = -Gx-

CFLAGS           = -c -Q -Si -G5 -O- -Oi- -Ss+ -W3 -Gm+ /Gn+ -Ti+ $(PROFILEFLAGS) $(CTARGETFLAGS)
CXXFLAGS         = -c -Q -Si -G5 -O- -Oi- -Ss+ -W3 -Gm+ /Gn+ -Ti+ $(PROFILEFLAGS) $(CXXTARGETFLAGS) -Gx+
CXXFLAGS_ODINCRT = -c -Q -Si -G5 -O- -Oi- -Ss+ -W3 -Gm+ /Gn- -Ti+ $(PROFILEFLAGS) $(CXXTARGETFLAGS) -Gx+
CFLAGS_WIN32APP  = -c -Q -Si -G5 -O- -Oi- -Ss+ -W3 -Gm+ /Gn+ -Ti+ $(PROFILEFLAGS) $(CTARGETFLAGS)
CXXFLAGS_WIN32APP= -c -Q -Si -G5 -O- -Oi- -Ss+ -W3 -Gm+ /Gn+ -Ti+ $(PROFILEFLAGS) $(CXXTARGETFLAGS)
!ifndef STATIC_CRT
CINCLUDES        = -I$(ODIN32_INCLUDE)\incl_vac -I$(ODIN32_INCLUDE)\Win -I. -I$(ODIN32_INCLUDE)
!else STATIC_CRT
CINCLUDES        = -I$(ODIN32_INCLUDE)\Win -I. -I$(ODIN32_INCLUDE)
!endif
CDEFINES         = -D__WIN32OS2__ -D__WINE__ -D__i386__ -DTCPV40HDRS -DCOMCTL32UNDOC
CDEFINES_ODINCRT = -D__WIN32OS2__ -D__WINE__ -D__i386__
CDEFINES_WIN32APP= -D__WIN32OS2__ -D__i386__


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
LDFLAGS          = -Q   -B"/noe /map /packcode /packdata /exepack:2 /linenumbers /dbgpack /nod" -Ti -Si -W3 -Gm+ $(PROFILEFLAGS) $(LDTARGETFLAGS)
LDFLAGS_ODINCRT  = -Q   -B"/noe /map /packcode /packdata /exepack:2 /linenumbers /dbgpack "     -Ti -Si -W3 -Gm+ $(PROFILEFLAGS) $(LDTARGETFLAGS)
LD2FLAGS         = /nologo /noe /map /packcode /packdata /exepack:2 /linenumbers /dbgpack /nod /debug $(LD2TARGETFLAGS)
LD2FLAGS_ODINCRT = /nologo /noe /map /packcode /packdata /exepack:2 /linenumbers /dbgpack      /debug $(LD2TARGETFLAGS)




!else
#
# This is Patricks take at profiling.
#   - Profile the DEBUG version of the system.
#


#
# Tool Flags
#
ASFLAGS          = -Mb -Sc -Sv:ALP +Od -D:DEBUG
IMPLIBFLAGS      = /nologo
ILIBFLAGS        = /nofree /quiet /nologo /noignorecase
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
!ifdef EXETARGET
CTARGETFLAGS     = -Ge+
CXXTARGETFLAGS   = -Ge+
!else
CTARGETFLAGS     = -Ge-
CXXTARGETFLAGS   = -Ge-
!endif
CXXEXCEPTIONS    = -Gx-
CFLAGS           = -c -Q -Si -Ti -Tm+ -Ss+ -W3 -Gm+ /Gn+ $(CTARGETFLAGS)
CXXFLAGS         = -c -Q -Si -Ti -Tm+ -Ss+ -W3 -Gm+ /Gn+ $(CXXTARGETFLAGS) /Gx+
CXXFLAGS_ODINCRT = -c -Q -Si -Ti -Tm+ -Ss+ -W3 -Gm+ /Gn- $(CXXTARGETFLAGS) /Gx+
CFLAGS_WIN32APP  = -c -Q -Si -Ti -Tm+ -Ss+ -W3 -Gm+ /Gn+ $(CTARGETFLAGS)
CXXFLAGS_WIN32APP= -c -Q -Si -Ti -Tm+ -Ss+ -W3 -Gm+ /Gn+ $(CXXTARGETFLAGS)
!ifndef STATIC_CRT
CINCLUDES        = -I$(ODIN32_INCLUDE)\incl_vac -I$(ODIN32_INCLUDE)\Win -I. -I$(ODIN32_INCLUDE)
!else
CINCLUDES        = -I$(ODIN32_INCLUDE)\Win -I. -I$(ODIN32_INCLUDE)
!endif
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
LD2TARGETFLAGS   = /EXEC /pmtype:vio /stack:$(STACKSIZE)
!   else
LDTARGETFLAGS    = -Ge+ -B"/pmtype:pm /stack:$(STACKSIZE)"
LD2TARGETFLAGS   = /EXEC /pmtype:pm /stack:$(STACKSIZE)
!   endif
!else
LDTARGETFLAGS    = -Ge-
LD2TARGETFLAGS   = /DLL
!endif
!ifdef NODEBUGINFO
LDFLAGS          = -Q   /B"/noe /map /packcode /packdata /exepack:2 /nodebug /nodbgpack /nod" -Si -W3 -Gm+ $(LDTARGETFLAGS)
LDFLAGS_ODINCRT  = -Q   /B"/noe /map /packcode /packdata /exepack:2 /nodebug /nodbgpack"      -Si -W3 -Gm+ $(LDTARGETFLAGS)
LD2FLAGS         = /nologo /noe /map /packcode /packdata /exepack:2 /nodebug /nodbgpack /nod $(LD2TARGETFLAGS)
LD2FLAGS_ODINCRT = /nologo /noe /map /packcode /packdata /exepack:2 /nodebug /nodbgpack      $(LD2TARGETFLAGS)
!else
LDFLAGS          = -Q   /B"/noe /map /packcode /packdata /exepack:2 /dbgpack /nod" -Ti -Si -W3 -Gm+ $(LDTARGETFLAGS)
LDFLAGS_ODINCRT  = -Q   /B"/noe /map /packcode /packdata /exepack:2 /dbgpack "     -Ti -Si -W3 -Gm+ $(LDTARGETFLAGS)
LD2FLAGS         = /nologo /noe /map /packcode /packdata /exepack:2 /dbgpack /nod /debug $(LD2TARGETFLAGS)
LD2FLAGS_ODINCRT = /nologo /noe /map /packcode /packdata /exepack:2 /dbgpack      /debug $(LD2TARGETFLAGS)
!endif

!endif