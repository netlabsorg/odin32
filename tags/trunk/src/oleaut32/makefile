#/* $Id: makefile,v 1.7 1999-11-14 21:05:23 davidr Exp $ */
#
# PD-Win32 API 
#
#	ole32.dll makefile
# 

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = oleaut32

OBJS =  oleaut32.obj guid.obj \
	initterm.obj olefont.obj \
	typelib.obj variant.obj \
	ole2disp.obj parsedt.obj \
	ipicture.obj \
	safearray.obj stubs.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def 
	$(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)\pmwinx.lib \
              $(PDWIN32_LIB)\advapi32.lib \
              $(PDWIN32_LIB)\kernel32.lib \
              $(PDWIN32_LIB)\user32.lib \
              $(PDWIN32_LIB)\ole32.lib \
              $(PDWIN32_LIB)\gdi32.lib \
               $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
	$(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@

oleaut32.obj:	oleaut32.cpp oleaut32.h
initterm.obj:	initterm.cpp
ole2disp.obj:    ole2disp.cpp oleaut32.h
variant.obj:    variant.cpp parsedt.h
typelib.obj:    typelib.cpp oleaut32.h
olefont.obj:    olefont.cpp oleaut32.h
ipicture.obj:   ipicture.cpp ipicture.h oleaut32.h
parsedt.obj:    parsedt.cpp parsedt.h
safearray.obj:  safearray.cpp oleaut32.h
guid.obj:       guid.cpp
stubs.obj:      stubs.cpp

clean:
	$(RM) *.obj *.lib *.dll *.map *.pch	
	$(RM) $(PDWIN32_BIN)\$(TARGET).dll
	$(RM) $(PDWIN32_LIB)\$(TARGET).lib




