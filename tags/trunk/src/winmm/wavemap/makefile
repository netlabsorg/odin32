# $Id: makefile,v 1.2 2000-01-18 20:24:02 sandervl Exp $

#
# PD-Win32 API
#
#	msacm32.drv makefile
#

PDWIN32_INCLUDE = ..\..\..\include
PDWIN32_LIB = ..\..\..\lib
PDWIN32_BIN = ..\..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = msacm32

OBJS =  msacm32.obj initterm.obj

all: $(TARGET).drv $(TARGET).lib


$(TARGET).drv: $(OBJS) $(TARGET).def
	$(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/user32.lib $(PDWIN32_LIB)/winmm.lib $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
	$(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET).def
	$(IMPLIB) $(IMPLIBFLAGS) $@ $**
	$(CP) $@ $(PDWIN32_LIB)


initterm.obj: initterm.cpp
msacm32.obj: msacm32.cpp

clean:
	$(RM) *.obj *.lib *.drv *~ *.map *.pch
	$(RM) $(PDWIN32_BIN)\$(TARGET).drv
	$(RM) $(PDWIN32_LIB)\$(TARGET).lib





