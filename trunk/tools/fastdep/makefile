# $Id: makefile,v 1.1 2000-02-09 23:48:49 bird Exp $

#
# PD-Win32 API
#
# Makefile for the Quick-and-Dirty dependency utility. (FastDep)
#
#


# include common definitions
PDWIN32_INCLUDE = ..\..\include
!include $(PDWIN32_INCLUDE)\pdwin32.mk

# Addjust common definitions
!ifdef DEBUG
CFLAGS   = $(CFLAGS)  -Ge+ -Tx+ -I$(PDWIN32_INCLUDE) -I..\common  \
           -W3 # -Wall+ppt-ppc-inl-cnv-gnr-vft- -O+
LDFLAGS  = $(LDFLAGS) -Ge+ -Fe$@ /B"/MAP:full"
!else
CFLAGS   = $(CFLAGS)  -Ge+ -Tx+ -I$(PDWIN32_INCLUDE) -I..\common      \
           -W3 # -Wall+ppt-ppc-inl-cnv-gnr-vft-
LDFLAGS  = $(LDFLAGS) -Ge+ -Fe$@ /B"/MAP:full"
!endif



all: fastdep.exe ..\bin\fastdep.exe


fastdep.exe: fastdep.obj
   $(LD) $(LDFLAGS) $** $(RTLLIB) os2386.lib

..\bin\fastdep.exe: fastdep.exe
   $(CP) $** $@

dep: # dummy rule

clean:
   -@$(RM) *.obj
   -@$(RM) *.exe
   -@$(RM) *.map
   -@$(RM) *.pch
   -@$(RM) *.log
