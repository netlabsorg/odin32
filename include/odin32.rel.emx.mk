# $Id: odin32.rel.emx.mk,v 1.8 2003-01-06 13:24:19 sandervl Exp $

#
# Odin32 API
#
#       RELEASE makefile for the GNU EMX compiler environment.
#
# ---------------------------------------------------------------------------------
# NOTE: this configuration is not supported neither working, it's experimental only.
# ---------------------------------------------------------------------------------


#
# Library macros.
#
SOMLIB   = somtk.lib
RTLLIB   = \
$(EMX)\lib\mt\c.lib \
$(EMX)\lib\mt\c_app.lib \
$(EMX)\lib\c_alias.lib \
$(EMX)\lib\mt\emx.lib \
$(EMX)\lib\emx2.lib \
$(EMX)\lib\stdcpp.lib \
$(EMX)\lib\gcc.lib \
$(EMX)\lib\end.lib

RTLLIB_O = $(EMX)\lib\mt\c_import.lib $(RTLIB)
RTLLIB_NRE =
DLLENTRY = $(ODIN32_LIB)\dllentry.obj
ODINCRT  = odincrtd
LIB_DEMANGLE =


#
# Tools
#
CC     = gcc
CXX    = gcc
AS     = alp
AS_EMX = as
LD     = gcc
LD2    = link386
IMPLIB = emximp
ILIB   = emxomfar
RC     = $(ODIN32_TOOLS)\wrc
OS2RC  = rc


#
# Tool Flags
#
ASFLAGS          = -Mb -Sc -Sv:ALP +Od -D:DEBUG
IMPLIBFLAGS      = -o
ILIBFLAGS        = rcv
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
CTARGETFLAGS     = -Zexe
CXXTARGETFLAGS   = -Zexe
!else
CTARGETFLAGS     = -Zdll
CXXTARGETFLAGS   = -Zdll
!endif
CXXEXCEPTIONS    = -fhandle-exceptions
CFLAGS           = -Zomf -pipe -Wall -Zmt -s -O6 -mcpu=pentium $(CTARGETFLAGS)
CXXFLAGS         = -Zomf -pipe -Wall -Zmt -s -O6 -mcpu=pentium $(CXXTARGETFLAGS)
CXXFLAGS_ODINCRT = -Zomf -pipe -Wall -Zmt -s -O6 -mcpu=pentium $(CXXTARGETFLAGS)
CFLAGS_WIN32APP  = -Zomf -pipe -Wall -Zmt -s -O6 -mcpu=pentium $(CTARGETFLAGS)
CXXFLAGS_WIN32APP= -Zomf -pipe -Wall -Zmt -s -O6 -mcpu=pentium $(CXXTARGETFLAGS) $(CXXEXCEPTIONS)
CINCLUDES        = -I$(ODIN32_INCLUDE)\Win -I. -I$(ODIN32_INCLUDE)
CDEFINES_WIN32APP= -D__WIN32OS2__ -D__i386__
CDEFINES_ODINCRT = -D__WIN32OS2__ -D__i386__ -D__WINE__
CDEFINES         = -D__WIN32OS2__ -D__i386__ -D__WINE__ -DTCPV40HDRS -DCOMCTL32UNDOC \
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
LDTARGETFLAGS    = -Zexe -Zstack 80
LD2TARGETFLAGS   = /pmtype:vio /stack:$(STACKSIZE)
!   else
LDTARGETFLAGS    = -Zexe -Zstack 80
LD2TARGETFLAGS   = /pmtype:pm  /stack:$(STACKSIZE)
!   endif
!else
LDTARGETFLAGS    = -Zdll
LD2TARGETFLAGS   = /DLL
!endif
LDFLAGS          = -Zomf -Zmap $(LDTARGETFLAGS)  -s 
LDFLAGS_ODINCRT  = -Zso -Zsys -Zomf -Zmap $(LDTARGETFLAGS)  -s 
LD2FLAGS         = /nologo /noe /map /packcode /packdata /exepack:2 /nodebug /nod $(LD2TARGETFLAGS)
LD2FLAGS_ODINCRT = /nologo /noe /map /packcode /packdata /exepack:2 /nodebug      $(LD2TARGETFLAGS)

