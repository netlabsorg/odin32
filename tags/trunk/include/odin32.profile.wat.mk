# $Id: odin32.profile.wat.mk,v 1.1 2000-12-09 16:03:38 phaller Exp $

#
# Odin32 API
#
#       DEBUG makefile for the Watcom C/C++ compiler environment.
#


#
# Library macros.
#
SOMLIB   = somtk.lib
RTLLIB   = clib3r.lib
RTLLIB_O = clib3r.lib
DLLENTRY = $(ODIN32_LIB)\dllentry_watcom.obj
ODINCRT  = odincrtd


#
# Object directory.
#
OBJDIR   = .\bin\debug.wat


#
# Tools
#
CXX    = WPP386
CC     = WCC386
AS     = alp
LD     = wlink
LD2    = $(LD)
IMPLIB = implib
ILIB   = ilib
RC     = $(ODIN32_TOOLS)\wrc
OS2RC  = rc


#
# Tool Flags
#
ASFLAGS          = -Mb -Sc -Sv:ALP +Od -D:DEBUG
IMPLIBFLAGS      = /nologo
ILIBFLAGS        = /quiet /nologo
RCFLAGS          = -s -I. -I$(%WATCOM)\include
OS2RCFLAGS       = -r -n
OS2RCLFLAGS      = -x2 -n


#
# Watcom compiler flags:
# w4            warning level 4
# wcd=726       disable warning 726 (unreferened local variable or parameter)
# e25           error limit
# d2            generate symbolic debug information
# od            no optimizations
# bm            multithreaded environment
# mf            flat memory model
# 5r            pentium register calling convention
# br            use dll for runtime library functions
# bt=os2        target os = OS/2
# zq            operate quietly (display only errors)
# fp5           optimize floating-point for pentium
# fpi87         inline fpu instructions
# hw            generate Watcom debug information
# xd        disable exception handling (default)
# xs        enable exception handling
# bd        build target is a dynamic link library (DLL)
CXXEXCEPTIONS    = -xs
!ifndef STATIC_CRT
DLLRTL = -br
!endif
CFLAGS           = -w4 -wcd=726 -e25 -od -d2 -hw -bm -mf -5r -bt=os2 -zq -fp5 -fpi87     $(DLLRTL)
CXXFLAGS         = -w4 -wcd=726 -e25 -od -d2 -hw -bm -mf -5r -bt=os2 -zq -fp5 -fpi87 -xd $(DLLRTL)
CXXFLAGS_ODINCRT = -w4 -wcd=726 -e25 -od -d2 -hw -bm -mf -5r -bt=os2 -zq -fp5 -fpi87 -xd
CFLAGS_WIN32APP  = -w4 -wcd=726 -e25 -od -d2 -hw -bm -mf -5r -bt=os2 -zq -fp5 -fpi87
CXXFLAGS_WIN32APP= -w4 -wcd=726 -e25 -od -d2 -hw -bm -mf -5r -bt=os2 -zq -fp5 -fpi87

CINCLUDES        = -i$(%WATCOM)\h -i$(ODIN32_INCLUDE)\Win -i. -i$(ODIN32_INCLUDE)
CDEFINES_WIN32APP= -DDEBUG -DPROFILE -D__WIN32OS2__ -D__i386__
CDEFINES_ODINCRT = -DDEBUG -DPROFILE -D__WIN32OS2__ -D__i386__ -D__WINE__
CDEFINES         = -DDEBUG -DPROFILE -D__WIN32OS2__ -D__i386__ -D__WINE__ -DTCPV40HDRS -DCOMCTL32UNDOC \
!ifdef DEBUGALL
                   -DDEBUG_ENABLELOG_LEVEL2 \
!endif
!ifdef NODEBUGINFO
                   -DDEFAULT_LOGGING_OFF
!endif

!ifdef EXETARGET
!ifdef VIO
CFLAGS           += -bc
CXXFLAGS         += -bc
!else
CFLAGS           += -bg
CXXFLAGS         += -bg
!endif
!else
CFLAGS           += -bd
CXXFLAGS         += -bd
!endif


#
# Linker flags.
#
LDFLAGS          = option maxe=125, quiet, symfile, nodefaultlibs
LDFLAGS_ODINCRT  = option maxe=125, quiet, symfile

!ifndef NODEBUGINFO
LDFLAGS          += debug all
LDFLAGS_ODINCRT  += debug all
!endif

LD2FLAGS         = $(LDFLAGS)
LD2FLAGS_ODINCRT = $(LDFLAGS_ODINCRT)

