# $Id: makefile,v 1.4 2000-03-16 21:10:10 bird Exp $

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
CFLAGS   = $(CFLAGS)  -Ge+ -Tx+ -I..\common \
           -W3 -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- -O+ -Tm-
#          -W3 -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- -Gh+
LDFLAGS  = $(LDFLAGS) -Ge+ -Fe$@ /B"/MAP:full"
!else
CFLAGS   = $(CFLAGS)  -Ge+ -Tx+ -I..\common      \
           -W3 -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext-
LDFLAGS  = $(LDFLAGS) -Ge+ -Fe$@ /B"/MAP:full"
!endif



all: fastdep.exe ..\bin\fastdep.exe


fastdep.exe: fastdep.obj avl.obj
    $(LD) $(LDFLAGS) $** $(RTLLIB) os2386.lib
#   $(LD) $(LDFLAGS) $** CPPOPA3.OBJ $(RTLLIB) os2386.lib

..\bin\fastdep.exe: fastdep.exe
   $(CP) $** $@


dep: # dummy rule


clean:
   -@$(RM) *.obj *.exe *.map *.pch *.log ..\bin\fastdep.exe
