# $Id: makefile,v 1.5 2000-02-05 02:04:28 sandervl Exp $

#
# PD-Win32 API
#
#	msvfw32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = msvfw32

OBJS =  msvfw32.obj resource.obj $(PDWIN32_LIB)/dllentry.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
	$(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/user32.lib $(PDWIN32_LIB)/gdi32.lib \
	      $(PDWIN32_LIB)/winmm.lib \
	      $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
	$(CP) $@ $(PDWIN32_BIN)

$(TARGET).lib: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@

resource.asm: $(TARGET).rc
    $(RC) $(RCFLAGS) -o resource.asm $(TARGET).rc

resource.obj: resource.asm

msvfw32.obj: msvfw32.cpp

clean:
	$(RM) *.obj *.lib *.dll *.map *.pch
	$(RM) $(PDWIN32_BIN)\$(TARGET).dll
	$(RM) $(PDWIN32_LIB)\$(TARGET).lib





