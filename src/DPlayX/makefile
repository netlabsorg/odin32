# $Id: makefile,v 1.2 1999-11-03 23:28:03 sandervl Exp $

#
# PD-Win32 API
#
#       dplayx.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = dplayx

OBJS =  dplayx.obj initterm.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O) \
              $(PDWIN32_LIB)/ole32.lib $(PDWIN32_LIB)/msvfw32.lib
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET).def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $**
        $(CP) $@ $(PDWIN32_LIB)


initterm.obj: initterm.cpp
dplayx.obj:   dplayx.cpp

clean:
        $(RM) *.obj *.lib *.dll *~ *.map *.pch
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib





