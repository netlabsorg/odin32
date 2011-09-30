# $Id: odin32.rel.vac36.mk,v 1.11 2003-02-28 10:28:11 sandervl Exp $

#
# Odin32 API
#
#       RELEASE makefile for the VAC 3.6.5 compiler environment.
#


#
# Library macros.
#
SOMLIB      = somtk.lib
RTLLIB      = cpprms36.lib
RTLLIB_O    = cpprmo36.lib
RTLLIB_NRE  = cpprni36.lib
DLLENTRY    = $(ODIN32_LIB)\dllentry.obj
ODINCRT     = odincrt
LIB_DEMANGLE = cpprds36.lib


#
# Tools
#
CC     = icc
AS     = alp
CXX    = icc
LD     = icc
LD2    = ilink
IMPLIB = implib
!ifdef USE_OLDLIB
ILIB   = libv2r1 /pagesize:64
!else
ILIB   = ilib
!endif
RC     = $(ODIN32_TOOLS)\wrc -M
OS2RC  = rc

!ifdef EXETARGET
STATIC_CRT=1
!endif

#
# Tool Flags
#
ASFLAGS          = -Mb -Sc -Sv:ALP
IMPLIBFLAGS      = /nologo
ILIBFLAGS        = /nofree /quiet /nologo /noignorecase
RCFLAGS          = -s -I. -I$(CXXMAIN)\include -D__WIN32OS2__
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
CFLAGS           = -c -Q -Si -G5 -O+ -Oi+ -Ss+ -W3 -Gm+ /Gn+ $(CTARGETFLAGS)
CXXFLAGS         = -c -Q -Si -G5 -O+ -Oi+ -Ss+ -W3 -Gm+ /Gn+ $(CXXTARGETFLAGS) /Gx+
CXXFLAGS_ODINCRT = -c -Q -Si -G5 -O+ -Oi+ -Ss+ -W3 -Gm+ /Gn- $(CXXTARGETFLAGS) /Gx+
CFLAGS_WIN32APP  = -c -Q -Si -G5 -O+ -Oi+ -Ss+ -W3 -Gm+ /Gn+ $(CTARGETFLAGS)
CXXFLAGS_WIN32APP= -c -Q -Si -G5 -O+ -Oi+ -Ss+ -W3 -Gm+ /Gn+ $(CXXTARGETFLAGS)
!ifndef STATIC_CRT
CINCLUDES        = -I$(ODIN32_INCLUDE)\incl_vac -I$(ODIN32_INCLUDE)\Win -I. -I$(ODIN32_INCLUDE)
!else
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
LD2TARGETFLAGS   = /EXEC /pmtype:vio /stack:$(STACKSIZE)
!   else
LDTARGETFLAGS    = -Ge+ -B"/pmtype:pm /stack:$(STACKSIZE)"
LD2TARGETFLAGS   = /EXEC /pmtype:pm /stack:$(STACKSIZE)
!   endif
!else
LDTARGETFLAGS    = -Ge-
LD2TARGETFLAGS   = /DLL
!endif
LDFLAGS          = -Q   /B"/noe /map /packcode /packdata /exepack:2 /nodebug /nod" $(LDTARGETFLAGS) -Si -G5 -O+ -W3 -Gm+
LDFLAGS_ODINCRT  = -Q   /B"/noe /map /packcode /packdata /exepack:2 /nodebug"      $(LDTARGETFLAGS) -Si -G5 -O+ -W3 -Gm+
LD2FLAGS         = /nologo /noe /map /packcode /packdata /exepack:2 /nodebug /nod $(LD2TARGETFLAGS)
LD2FLAGS_ODINCRT = /nologo /noe /map /packcode /packdata /exepack:2 /nodebug      $(LD2TARGETFLAGS)

