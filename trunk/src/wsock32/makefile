# $Id: makefile,v 1.5 1999-08-09 19:00:24 achimha Exp $

#
# PD-Win32 API
#
#	wsock32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = wsock32

OBJS =  wsock32.obj notify.obj initterm.obj unknown.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
	$(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) sbsstub.obj $(TARGET).def \
              $(PDWIN32_LIB)\pmwinx.lib $(PDWIN32_LIB)\user32.lib \
              tcp32dll.lib so32dll.lib  $(PDWIN32_LIB)\kernel32.lib \
              $(PDWIN32_LIB)/odincrt.lib OS2386.LIB
	$(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET).dll
	$(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET).dll
        $(CP) $@ $(PDWIN32_LIB)


wsock32.obj: wsock32.cpp wsock32.h
notify.obj:  notify.cpp wsock32.h
unknown.obj: unknown.cpp wsock32.h
initterm.obj: initterm.cpp

clean:
	$(RM) *.obj *.lib *.dll *~ *.map *.pch	
	$(RM) $(PDWIN32_BIN)\$(TARGET).dll
	$(RM) $(PDWIN32_LIB)\$(TARGET).lib

