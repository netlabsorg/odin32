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
LDFLAGS = $(LDFLAGS) -Ge+ /B"/pmtype:pm /stack:0x30000 /NOBASE /Map"

TARGET = pe

OBJS =  pe.obj \
        ..\kernel32\kernel32.lib

all: $(TARGET).exe


$(TARGET).exe: $(OBJS)
        $(LD) $(LDFLAGS) -Fe$@ $(OBJS)
        $(CP) $(TARGET).exe $(PDWIN32_BIN)



misc.obj: misc.cpp
pe.obj: pe.cpp pe.h $(PDWIN32_INCLUDE)\winimage.h $(PDWIN32_INCLUDE)\winexe.h

clean:
        $(RM) *.obj *.lib *.dll *~ *.map *.pch *.exe
        $(RM) $(PDWIN32_BIN)\$(TARGET).exe
