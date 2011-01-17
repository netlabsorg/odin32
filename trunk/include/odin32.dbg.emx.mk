# $Id: odin32.dbg.emx.mk,v 1.10 2003-02-28 10:28:09 sandervl Exp $

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
RTLLIB   = \
#$(EMX)\lib\mt\c.lib \
#$(EMX)\lib\mt\c_app.lib \
#$(EMX)\lib\c_alias.lib \
#$(EMX)\lib\mt\emx.lib \
#$(EMX)\lib\emx2.lib \
#$(EMX)\lib\stdcpp.lib \
#$(EMX)\lib\gcc.lib \
$(ODIN32_BASE)\src\msvcrt\libs\m.lib \
$(EMX)\lib\iberty_s.lib \
$(EMX)\lib\gcc-lib\i386-pc-os2_emx\3.0.3\mt\gcc.lib \
$(EMX)\lib\mt\c.lib \
$(EMX)\lib\mt\c_dllso.lib \
$(EMX)\lib\mt\sys.lib \
$(EMX)\lib\c_alias.lib \
$(EMX)\lib\mt\emx.lib \
$(EMX)\lib\emx2.lib \
$(EMX)\lib\gcc-lib\i386-pc-os2_emx\3.0.3\mt\stdcxx.lib \
$(EMX)\lib\gcc-lib\i386-pc-os2_emx\3.0.3\mt\gpp.lib \
$(EMX)\lib\stdcxx.lib \
#$(EMX)\lib\stdcpp.lib \
#$(EMX)\lib\mt\c_app.lib makes trouble for crtdll \

RTLLIB_O = $(EMX)\lib\mt\c_import.lib $(RTLLIB)
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
LD     = gcc
LD2    = gcc
IMPLIB = implib
ILIB   = emxomfar
RC     = $(ODIN32_TOOLS)\wrc
OS2RC  = rc


#
# Tool Flags
#
ASFLAGS          = -Mb -Sc -Sv:ALP +Od -D:DEBUG
IMPLIBFLAGS      = /nologo
ILIBFLAGS        = rcv
RCFLAGS          = -s -I. -I$(CPPMAIN)\include  -D__WIN32OS2__
!ifdef WRC_PREFIX_RESOURCE
RCFLAGS          = $(RCFLAGS) -p _$(TARGET)
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
CFLAGS           = -Zomf -pipe -Wall -Zmt -g $(CTARGETFLAGS)
CXXFLAGS         = -Zomf -pipe -Wall -Zmt -g $(CXXTARGETFLAGS)
CXXFLAGS_ODINCRT = -Zomf -pipe -Wall -Zmt -g $(CXXTARGETFLAGS)
CFLAGS_WIN32APP  = -Zomf -pipe -Wall -Zmt -g $(CTARGETFLAGS)
CXXFLAGS_WIN32APP= -Zomf -pipe -Wall -Zmt -g $(CXXTARGETFLAGS) $(CXXEXCEPTIONS)
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
LDTARGETFLAGS    = -Zdll -Zso -Zsys
LD2TARGETFLAGS   = -Zdll
!endif
!ifdef NODEBUGINFO
LDFLAGS          = -Zmt -Zomf -Zmap -Zlinker "/MAP:FULL /Linenumbers /exepack:2 /Nod" $(LDTARGETFLAGS) -s
LDFLAGS_ODINCRT  = -Zmt -Zomf -Zmap -Zlinker "/MAP:FULL /Linenumbers /exepack:2     " $(LDTARGETFLAGS) -s
#LD2FLAGS         = /nologo /noe /map:full /NoIgnoreCase /Linenumbers /packcode /packdata /exepack:2 /nodebug /nod $(LD2TARGETFLAGS)
LD2FLAGS         = -Zmap -s $(LD2TARGETFLAGS)
LD2FLAGS_ODINCRT = /nologo /noe /map:full /NoIgnoreCase /Linenumbers /packcode /packdata /exepack:2 /nodebug      $(LD2TARGETFLAGS)
LD2FLAGS_ODINCRT         = -Zmap -s $(LD2TARGETFLAGS)

!else
LDFLAGS          = -Zmt -Zomf -Zmap -Zlinker "/MAP:FULL /Linenumbers /exepack:2 /Nod" $(LDTARGETFLAGS) -g
LDFLAGS_ODINCRT  = -Zmt -Zomf -Zmap -Zlinker "/MAP:FULL /Linenumbers /exepack:2     " $(LDTARGETFLAGS) -g
LD2FLAGS         = -g -Zomf -Zmap -Zhigh-mem $(LD2TARGETFLAGS)
LD2FLAGS_ODINCRT = -g -Zomf -Zmap -Zhigh-mem $(LD2TARGETFLAGS)
!endif

