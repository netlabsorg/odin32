# $Id: makefile,v 1.1 1999-09-05 02:25:48 bird Exp $

#
# PD-Win32 API
#
# Makefile for the ImpDef utility.
#
#


# include common definitions
PDWIN32_INCLUDE = ..\..\include
!include $(PDWIN32_INCLUDE)\pdwin32.mk

# Addjust common definitions
!ifdef DEBUG
CFLAGS   = $(CFLAGS)   -Ge+ -Tx+ -I$(PDWIN32_INCLUDE) -I..\common  \
           -Wall+ppt-ppc-inl-cnv-gnr-vft-
CXXFLAGS = $(CXXFLAGS) -Ge+ -Gx- -Tx+ -I$(PDWIN32_INCLUDE) -I..\common \
           -Wall+ppt-ppc-inl-cnv-gnr-vft-
LDFLAGS  = $(LDFLAGS)  -Ge+ -Fe$@ /B"/MAP:full"
!else
CFLAGS   = $(CFLAGS)   -Ge+ -Tx+ -I$(PDWIN32_INCLUDE) -I..\common      \
           -Wall+ppt-ppc-inl-cnv-gnr-vft-
CXXFLAGS = $(CXXFLAGS) -Ge+ -Gx- -Tx+ -I$(PDWIN32_INCLUDE) -I..\common \
           -Wall+ppt-ppc-inl-cnv-gnr-vft-
LDFLAGS  = $(LDFLAGS)  -Ge+ -Fe$@ /B"/MAP:full"
!endif



all: ImpDef.exe ..\bin\ImpDef.exe


ImpDef.exe: ImpDef.obj ..\common\commonicc.lib
   $(LD) $(LDFLAGS) $** cppom30.lib os2386.lib

..\bin\ImpDef.exe: ImpDef.exe
   $(CP) $** $@

..\common\commonicc.lib:
   @cd ..\common
   @nmake /nologo commonicc.lib
   @cd ..\impdef


clean:
   -@$(RM) *.obj
   -@$(RM) *.exe
   -@$(RM) *.map
   -@$(RM) *.pch
   -@$(RM) *.log


# Dependencies
ImpDef.obj:    ImpDef.cpp ImpDef.h ..\common\kFileFormatBase.h \
               ..\common\kFileDef.h

