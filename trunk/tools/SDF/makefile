# $Id: makefile,v 1.1 2000-02-09 23:49:29 bird Exp $

#
# PD-Win32 API
#
# Makefile for the SDF utility.
#
#


# include common definitions
PDWIN32_INCLUDE = ..\..\include
PDWIN32_TOOLS   = ..\bin
!include $(PDWIN32_INCLUDE)\pdwin32.mk

# Addjust common definitions
CINCLUDES= -I$(PDWIN32_INCLUDE) -I$(PDWIN32_INCLUDE)\win -I..\common
!ifdef DEBUG
CFLAGS   = $(CFLAGS)   -Ge+ -Tx+ $(CINCLUDES) -Wall+ppt-ppc-inl-cnv-gnr-vft-
CXXFLAGS = $(CXXFLAGS) -Ge+ -Gx- -Tx+ $(CINCLUDES) -Wall+ppt-ppc-inl-cnv-gnr-vft-
LDFLAGS  = $(LDFLAGS)  -Ge+ -Fe$@ /B"/MAP:full"
!else
CFLAGS   = $(CFLAGS)   -Ge+ -Tx+ $(CINCLUDES) -Wall+ppt-ppc-inl-cnv-gnr-vft-
CXXFLAGS = $(CXXFLAGS) -Ge+ -Gx- -Tx+ $(CINCLUDES) -Wall+ppt-ppc-inl-cnv-gnr-vft-
LDFLAGS  = $(LDFLAGS)  -Ge+ -Fe$@ /B"/MAP:full"
!endif



all: sdf.exe ..\bin\sdf.exe


sdf.exe: sdf.obj ..\common\commonicc.lib
   $(LD) $(LDFLAGS) $** $(RTLLIB) os2386.lib

..\bin\sdf.exe: sdf.exe
   $(CP) $** $@

dep:
    $(DEPEND) $(CINCLUDES) -I *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h \
        *.c *.cpp

!ifndef NODEP
!include .depend
!endif


clean:
    @$(RM) *.obj *.exe *.map *.pch *.log

