#
# PD-Win32 API 
#
#	comdlg32.dll makefile
# 

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = comdlg32

OBJS =  comdlg32.obj initterm.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def 
	$(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/user32.lib
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET)exp.def
	$(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
	$(CP) $@ $(PDWIN32_LIB)

comdlg32.obj: comdlg32.cpp
initterm.obj: initterm.cpp

clean:
	$(RM) *.obj *.lib *.dll *~ *.map *.pch	
	$(RM) $(PDWIN32_BIN)\$(TARGET).dll
	$(RM) $(PDWIN32_LIB)\$(TARGET).lib

