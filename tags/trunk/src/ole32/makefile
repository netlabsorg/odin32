# $Id: makefile,v 1.1 1999-05-24 20:19:52 ktk Exp $ 

#
# PD-Win32 API 
#
#	ole32.dll makefile
# 

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = ole32

OBJS =  ole32.obj initterm.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def 
	$(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib
	$(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET).dll
	$(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET).dll
	$(CP) $@ $(PDWIN32_LIB)




ole32.obj: ole32.cpp ole32.h
initterm.obj: initterm.c


clean:
	$(RM) *.obj *.lib *.dll *~ *.map *.pch	
	$(RM) $(PDWIN32_BIN)\$(TARGET).dll
	$(RM) $(PDWIN32_LIB)\$(TARGET).lib


