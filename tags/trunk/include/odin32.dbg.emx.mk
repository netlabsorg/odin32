# $Id: odin32.dbg.emx.mk,v 1.3 2001-04-02 09:54:45 sandervl Exp $

#
# Odin32 API
#
#       DEBUG makefile for the GNU EMX compiler environment.
#
# ---------------------------------------------------------------------------------
# NOTE: this configuration is not supported neither working, it's experimental only.
# ---------------------------------------------------------------------------------


#
# Library macros.
#
SOMLIB   = somtk.lib
RTLLIB   =
RTLLIB_O =
#RTLLIB_NRE =
DLLENTRY = $(ODIN32_LIB)\dllentry.obj
ODINCRT  = odincrtd


#
# Tools
#
CC     = gcc
CXX    = gcc
AS     = alp
LD     = gcc
LD2    = ld
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
RCFLAGS          = -s -I. -I$(CPPMAIN)\include  -D__WIN32OS2__
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
CFLAGS           = -mprobe -pg -c -pipe -Wall -Zmt -g $(CTARGETFLAGS)
CXXFLAGS         = -mprobe -pg -c -pipe -Wall -Zmt -g $(CXXTARGETFLAGS)
CXXFLAGS_ODINCRT = -mprobe -pg -c -pipe -Wall -Zmt -g $(CXXTARGETFLAGS)
CFLAGS_WIN32APP  = -mprobe -pg -c -pipe -Wall -Zmt -g $(CTARGETFLAGS)
CXXFLAGS_WIN32APP= -mprobe -pg -c -pipe -Wall -Zmt -g $(CXXTARGETFLAGS) $(CXXEXCEPTIONS)
CINCLUDES        = -I$(ODIN32_INCLUDE)\Win -I. -I$(ODIN32_INCLUDE)
CDEFINES_WIN32APP= -DDEBUG -D__WIN32OS2__ -D__i386__
CDEFINES_ODINCRT = -DDEBUG -D__WIN32OS2__ -D__i386__ -D__WINE__
CDEFINES         = -DDEBUG -D__WIN32OS2__ -D__i386__ -D__WINE__ -DTCPV40HDRS -DCOMCTL32UNDOC \
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
# STACKSIZE is not supported.
!   ifdef VIO
LDTARGETFLAGS    = -Zexe /Zstack 80
LD2TARGETFLAGS   = -Zexe /Zstack 80
!   else
LDTARGETFLAGS    = -Zexe /Zstack 80
LD2TARGETFLAGS   = -Zexe /Zstack 80
!   endif
!else
LDTARGETFLAGS    = -Zdll
LD2TARGETFLAGS   = -Zdll
!endif
!ifdef NODEBUGINFO
LDFLAGS          = -Zmt -Zomf -Zmap $(LDTARGETFLAGS)  -s -ZLinker /EXEPACK:2 -ZLinker /NOD
LDFLAGS_ODINCRT  = -Zmt -Zomf -Zmap $(LDTARGETFLAGS)  -s -ZLinker /EXEPACK:2
LD2FLAGS         = -Zmt -Zomf -Zmap $(LD2TARGETFLAGS) -s -O/EXEPACK:2        -O/NOD
LD2FLAGS_ODINCRT = -Zmt -Zomf -Zmap $(LD2TARGETFLAGS) -s -O/EXEPACK:2
!else
LDFLAGS          = -Zmt -Zomf -Zmap $(LDTARGETFLAGS)  -ZLinker /NOD
LDFLAGS_ODINCRT  = -Zmt -Zomf -Zmap $(LDTARGETFLAGS)
LD2FLAGS         = -Zmt -Zomf -Zmap $(LD2TARGETFLAGS) -O/NOD
LD2FLAGS_ODINCRT = -Zmt -Zomf -Zmap $(LD2TARGETFLAGS)
!endif

