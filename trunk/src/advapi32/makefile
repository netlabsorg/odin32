# $Id: makefile,v 1.17 1999-12-27 18:44:33 sandervl Exp $

#
# PD-Win32 API
#
#       advapi32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk

CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = advapi32

OBJS =  advapi32.obj initterm.obj registry.obj lsa.obj security.obj service.obj eventlog.obj crypt.obj


all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/kernel32.lib $(PDWIN32_LIB)/ntdll.lib \
              $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
    $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@


advapi32.obj: advapi32.cpp advapi32.h $(PDWIN32_INCLUDE)\odinwrap.h
registry.obj: registry.cpp $(PDWIN32_INCLUDE)\odinwrap.h
lsa.obj:      lsa.cpp $(PDWIN32_INCLUDE)\odinwrap.h
initterm.obj: initterm.cpp
service.obj:  service.cpp
crypt.obj:  crypt.cpp
eventlog.obj:  eventlog.cpp

clean:
    $(RM) *.obj *.lib *.dll *.map *.pch
    $(RM) $(PDWIN32_BIN)\$(TARGET).dll
    $(RM) $(PDWIN32_LIB)\$(TARGET).lib
    $(RM) $(TARGET)exp.def
