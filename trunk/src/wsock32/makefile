# $Id: makefile,v 1.16 2000-02-05 02:19:44 sandervl Exp $

#
# PD-Win32 API
#
#	wsock32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin

!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = wsock32

OBJS =  wsock32.obj resource.obj unknown.obj relaywin.obj $(PDWIN32_LIB)/dllentry.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
	$(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              pmwsock.lib $(PDWIN32_LIB)\kernel32.lib \
              $(PDWIN32_LIB)\user32.lib  \
              $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
	$(CP) $@ $(PDWIN32_BIN)

$(TARGET).lib: $(TARGET)exp.def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
    $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
    $(IMPDEF) $** $@

resource.asm: $(TARGET).rc
    $(RC) $(RCFLAGS) -o resource.asm $(TARGET).rc

resource.obj: resource.asm

wsock32.obj: wsock32.cpp   relaywin.h wsock32.h
unknown.obj: unknown.cpp   wsock32.h
relaywin.obj: relaywin.cpp relaywin.h wsock32.h


clean:
	$(RM) *.obj *.lib *.dll *~ *.map *.pch
	$(RM) $(PDWIN32_BIN)\$(TARGET).dll
	$(RM) $(PDWIN32_LIB)\$(TARGET).lib

