# $Id: makefile,v 1.3 1999-11-03 23:28:06 sandervl Exp $

#
# PD-Win32 API
#
#       avifil32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = wnaspi32

OBJS =  odinaspi.obj winaspi32.obj initterm.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O) \
              $(PDWIN32_LIB)/advapi32.lib
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET).def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $**
        $(CP) $@ $(PDWIN32_LIB)


initterm.obj:  initterm.cpp
ainaspi32.obj: wnaspi32.cpp odinaspi.h srbos2.h
odinaspi.obj:  odinaspi.cpp odinaspi.h srbos2.h
clean:
        $(RM) *.obj *.lib *.dll *~ *.map *.pch
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib





