# $Id: makefile,v 1.4 1999-08-09 21:03:19 phaller Exp $

#
# PD-Win32 API
#
#       pe.exe makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -Ge+ /Gd-  -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -Ge+ /Gd- -I$(PDWIN32_INCLUDE)
LDFLAGS = $(LDFLAGS) -Ge+ /B"/pmtype:pm /stack:0x30000 /NOBASE /Map" \
          ..\..\lib\kernel32.lib os2386.lib ..\..\lib\odincrt.lib cppom30.lib

TARGET = pe

OBJS =  pe.obj

all: $(TARGET).exe


$(TARGET).exe: $(OBJS)
        $(LD) $(LDFLAGS) -Fe$@ $(OBJS)
        $(CP) $(TARGET).exe $(PDWIN32_BIN)



misc.obj: misc.cpp
pe.obj: pe.cpp pe.h $(PDWIN32_INCLUDE)\winimage.h $(PDWIN32_INCLUDE)\winexe.h

clean:
        $(RM) *.obj *.lib *.dll *~ *.map *.pch *.exe
        $(RM) $(PDWIN32_BIN)\$(TARGET).exe
