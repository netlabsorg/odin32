# $Id: odin32.dbg.watn.mk,v 1.1 2001-10-01 01:19:56 bird Exp $

#
# Odin32 API
#
#       DEBUG makefile for the Watcom C/C++ compiler environment.  (NMAKE)
#


#
# Rule attributes.
#
SYMBOLIC =


#
# Library macros.
#
SOMLIB   = somtk.lib
RTLLIB   = clib3r.lib
RTLLIB_O = clib3r.lib plbrdll.lib mt7rdll.lib
RTLLIB_NRE = clib3r.lib
DLLENTRY = $(ODIN32_LIB)\dllentry.obj
ODINCRT  = odincrtd


#
# Tools
#
CXX    = WPP386
CC     = WCC386
AS     = alp
LD     = wlink
LD2    = $(LD)
IMPLIB = implib
ILIB   = wlib
RC     = $(ODIN32_TOOLS)\wrc
OS2RC  = rc


#
# Tool Flags
#
ASFLAGS          = -Mb -Sc -Sv:ALP +Od -D:DEBUG
IMPLIBFLAGS      = /nologo
ILIBFLAGS        = -q -b -c -fo
RCFLAGS          = -s -I. -I$(WATCOM)\include -D__WIN32OS2__
!ifdef WRC_PREFIX_RESOURCE
RCFLAGS          = $(RCFLAGS) -p $(TARGET)
!endif
OS2RCFLAGS       = -r -n
OS2RCLFLAGS      = -x2 -n


#
# Watcom compiler flags:
# w4        warning level 4
# wcd=726   disable warning 726 (unreferened local variable or parameter)
# e25       error limit
# d2        generate symbolic debug information
# od        no optimizations
# bm        multithreaded environment
# mf        flat memory model
# 5r        pentium register calling convention
# br        use dll for runtime library functions
# bt=os2    target os = OS/2
# zq        operate quietly (display only errors)
# fp5       optimize floating-point for pentium
# fpi87     inline fpu instructions
# hw        generate Watcom debug information
# xd        disable exception handling (default)
# xs        enable exception handling
# bd        build target is a dynamic link library (DLL)
# bc        build target is a console application (EXE)
# bg        build target is a GUI application
CXXEXCEPTIONS    = -xs
!ifndef STATIC_CRT
DLLRTL           = -br
!endif
!ifdef EXETARGET
!   ifdef VIO
CTARGET          = -bc
CXXTARGET        = -bc
!   else
CTARGET          = -bg
CXXTARGET        = -bg
!   endif
!else
CTARGET          = -bd
CXXTARGET        = -bd
!endif

CFLAGS           = -w4 -wcd=726 -e25 -od -d2 -hw -bm -mf -5r -bt=os2 -zq -fp5 -fpi87     $(CTARGET)   $(DLLRTL)
CXXFLAGS         = -w4 -wcd=726 -e25 -od -d2 -hw -bm -mf -5r -bt=os2 -zq -fp5 -fpi87 -xd $(CXXTARGET) $(DLLRTL)
CXXFLAGS_ODINCRT = -w4 -wcd=726 -e25 -od -d2 -hw -bm -mf -5r -bt=os2 -zq -fp5 -fpi87 -xd $(CXXTARGET)
CFLAGS_WIN32APP  = -w4 -wcd=726 -e25 -od -d2 -hw -bm -mf -5r -bt=os2 -zq -fp5 -fpi87     $(CTARGET)
CXXFLAGS_WIN32APP= -w4 -wcd=726 -e25 -od -d2 -hw -bm -mf -5r -bt=os2 -zq -fp5 -fpi87     $(CXXTARGET)

CINCLUDES        = -I$(WATCOM)\h -I$(ODIN32_INCLUDE)\Win -I. -I$(ODIN32_INCLUDE)
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
#
!ifdef EXETARGET
!   ifndef STACKSIZE
STACKSIZE = 0x50000
!   endif
LDSTACK          = option stack=$(STACKSIZE)
!endif
!ifndef NODEBUGINFO
LDDEBUG          = debug all
!endif

LDFLAGS          = option maxe=125, caseexact, quiet, symfile, nodefaultlibs  LIBPath $(WATCOM)\lib386\os2;$(WATCOM)\lib386 $(LDSTACK) $(LDDEBUG)
LDFLAGS_ODINCRT  = option maxe=125, caseexact, quiet, symfile  LIBPath $(WATCOM)\lib386\os2;$(WATCOM)\lib386                $(LDSTACK) $(LDDEBUG)

LD2FLAGS         = $(LDFLAGS)
LD2FLAGS_ODINCRT = $(LDFLAGS_ODINCRT)

