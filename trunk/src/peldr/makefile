# $Id: makefile,v 1.12 2000-01-30 14:48:51 sandervl Exp $

#
# PD-Win32 API
#
#       pe.exe makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk

CFLAGS   = $(CFLAGS) /Ge+ -I$(PDWIN32_INCLUDE) -Gn-
CXXFLAGS = $(CXXFLAGS) /Ge+ -I$(PDWIN32_INCLUDE) -Gn-
LDFLAGS  = $(LDFLAGS_ODINCRT) /Ge+ /B"/pmtype:pm /stack:0x100000 /NOBASE /Map" \
             os2386.lib


TARGET = pe

OBJS =  pe.obj

all: $(TARGET).exe


$(TARGET).exe: $(OBJS) makefile
        $(LD) $(LDFLAGS) -Fe$@ $(OBJS)
        $(CP) $(TARGET).exe $(PDWIN32_BIN)
	$(CP) $@ $(PDWIN32_BIN)


pe.obj: pe.cpp pe.h $(PDWIN32_INCLUDE)\winimagebase.h $(PDWIN32_INCLUDE)\winexebase.h

clean:
        $(RM) *.obj *.lib *.dll *.map *.pch *.exe
        $(RM) $(PDWIN32_BIN)\$(TARGET).exe
