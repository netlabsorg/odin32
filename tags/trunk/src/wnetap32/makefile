# $Id: makefile,v 1.6 1999-08-21 12:29:32 sandervl Exp $

#
# PD-Win32 API
#
#	wnetap32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = wnetap32

OBJS =  wnetap32.obj initterm.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
	$(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/user32.lib $(PDWIN32_LIB)/odincrt.lib \
              OS2386.LIB $(RTLLIB_O)
	$(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET).dll
	$(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET).dll
	$(CP) $@ $(PDWIN32_LIB)


wnetap32.obj: wnetap32.cpp
initterm.obj: initterm.cpp


clean:
	$(RM) *.obj *.lib *.dll *~ *.map *.pch	
	$(RM) $(PDWIN32_BIN)\$(TARGET).dll
	$(RM) $(PDWIN32_LIB)\$(TARGET).lib


