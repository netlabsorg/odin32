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

OBJS =  ole32.obj \
        moniker.obj \
	initterm.obj \
	oString.obj \
	clsid.obj \
	guid.obj \
	initialise.obj \
	taskmem.obj \
	stubs.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def 
	$(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)\pmwinx.lib \
              $(PDWIN32_LIB)\advapi32.lib \
              $(PDWIN32_LIB)\kernel32.lib
	$(CP) $@ $(PDWIN32_BIN)
#              $(PDWIN32_LIB)\ntdll.lib \


$(TARGET).lib: $(TARGET).dll
	$(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET).dll
	$(CP) $@ $(PDWIN32_LIB)


oString.obj:	oString.cpp 
ole32.obj:	ole32.cpp 
initialise.obj:	initialise.cpp 
clsid.obj:	clsid.cpp 
guid.obj:	guid.cpp 
stubs.obj:	stubs.cpp 
initterm.obj:	initterm.cpp
moniker.obj:	moniker.cpp
taskmem.obj:	taskmem.cpp


clean:
	$(RM) *.obj *.lib *.dll *~ *.map *.pch	
	$(RM) $(PDWIN32_BIN)\$(TARGET).dll
	$(RM) $(PDWIN32_LIB)\$(TARGET).lib


