# $Id: makefile,v 1.7 2000-02-05 01:55:11 sandervl Exp $

#
# PD-Win32 API
#
#	crtdll.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE) 
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = crtdll

OBJS =  crtdll.obj asmhlp.obj stubs.obj internal.obj crt_string.obj \
	crt_memory.obj crt_mb.obj crt_wc.obj initterm.obj resource.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
	$(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib \
	       $(PDWIN32_LIB)/odincrt.lib $(PDWIN32_LIB)/user32.lib OS2386.LIB $(RTLLIB_O) 
	$(CP) $@ $(PDWIN32_BIN)


$(TARGET).LIB: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@

resource.asm: $(TARGET).rc
    $(RC) $(RCFLAGS) -o resource.asm $(TARGET).rc

resource.obj: resource.asm

initterm.obj:	initterm.cpp $(PDWIN32_INCLUDE)\exitlist.h
asmhlp.obj: 	asmhlp.asm
stubs.obj:	stubs.cpp
internal.obj:	internal.cpp
crt_string.obj:	crt_string.cpp
crt_mb.obj:	crt_mb.cpp
crt_memory.obj:	crt_memory.cpp
crt_wc.obj:	crt_wc.cpp

crtdll.obj:	crtdll.cpp

clean:
	$(RM) *.obj *.lib *.dll *.map *.pch
	$(RM) $(PDWIN32_BIN)\$(TARGET).dll
	$(RM) $(PDWIN32_LIB)\$(TARGET).lib





