# $Id: odin32.rel.wat.mk,v 1.7 2002-02-23 23:51:50 bird Exp $

#
# Odin32 API
#
#       RELEASE makefile for the Watcom C/C++ compiler environment.
#


#
# Rule attributes.
#
SYMBOLIC = .SYMBOLIC


#
# Library macros.
#
SOMLIB   = somtk.lib
RTLLIB   = clib3r.lib
RTLLIB_O = clib3r.lib plbrdll.lib mt7rdll.lib
RTLLIB_NRE = clib3r.lib
DLLENTRY = $(ODIN32_LIB)\dllentry.obj
ODINCRT  = odincrtd
LIB_DEMANGLE =


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
ASFLAGS          = -Mb -Sc -Sv:ALP
IMPLIBFLAGS      = /nologo
ILIBFLAGS        = /quiet /nologo
RCFLAGS          = -s -I. -I$(%WATCOM)\include -D__WIN32OS2__
!ifdef WRC_PREFIX_RESOURCE
RCFLAGS         += -p $(TARGET)
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
# ez        generated OMF objects
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
CXXEXCEPTIONS    = -xs
!ifndef STATIC_CRT
DLLRTL = -br
!endif
CFLAGS           = -w4 -wcd=726 -e25 -od -bm -mf -5r -bt=os2 -zq -fp5 -fpi87 -s     $(DLLRTL)
CXXFLAGS         = -w4 -wcd=726 -e25 -od -bm -mf -5r -bt=os2 -zq -fp5 -fpi87 -s -xd $(DLLRTL)
CXXFLAGS_ODINCRT = -w4 -wcd=726 -e25 -od -bm -mf -5r -bt=os2 -zq -fp5 -fpi87 -s -xd
CFLAGS_WIN32APP  = -w4 -wcd=726 -e25 -od -bm -mf -5r -bt=os2 -zq -fp5 -fpi87
CXXFLAGS_WIN32APP= -w4 -wcd=726 -e25 -od -bm -mf -5r -bt=os2 -zq -fp5 -fpi87
CINCLUDES        = -I$(%WATCOM)\h -I$(ODIN32_INCLUDE)\Win -I. -I$(ODIN32_INCLUDE)
CDEFINES         = -D__WIN32OS2__ -D__WINE__ -D__i386__ -DTCPV40HDRS -DCOMCTL32UNDOC
CDEFINES_ODINCRT = -D__WIN32OS2__ -D__WINE__ -D__i386__
CDEFINES_WIN32APP= -D__WIN32OS2__ -D__i386__

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
!ifndef STACKSIZE
STACKSIZE = 0x50000
!endif
LDFLAGS          = option nodefaultlibs, maxe=25, q, symf
LDFLAGS_ODINCRT  = option maxe=25, q, symf

!ifdef EXETARGET
LDFLAGS          += option stack=$(STACKSIZE)
LDFLAGS_ODINCRT  += option stack=$(STACKSIZE)
!endif

LD2FLAGS         = $(LDFLAGS)
LD2FLAGS_ODINCRT = $(LDFLAGS_ODINCRT)

