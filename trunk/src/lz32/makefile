# $Id: makefile,v 1.8 2000-02-05 02:01:38 sandervl Exp $

#
# PD-Win32 API
#
#       lz32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)
IMPLIBFLAGS = $(IMPLIBFLAGS)

TARGET = lz32

OBJS =  lz32.obj resource.obj $(PDWIN32_LIB)/dllentry.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET)exp.def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) \
        $(PDWIN32_LIB)\odincrt.lib \
        $(PDWIN32_LIB)\kernel32.lib \
        os2386.lib \
        $(RTLLIB_O) \
        $(TARGET).def
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@

$(TARGET).lib: $(TARGET)exp.def
	$(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
	$(CP) $@ $(PDWIN32_LIB)


resource.asm: $(TARGET).rc
    $(RC) $(RCFLAGS) -o resource.asm $(TARGET).rc

resource.obj: resource.asm

lz32.obj: lz32.cpp lz32.h $(PDWIN32_INCLUDE)\win\debugtools.h

clean:
        $(RM) *exp.def *.obj *.lib *.dll *.map *.pch
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib

