#
# PD-Win32 API
#
# wing32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin

!include $(PDWIN32_INCLUDE)/pdwin32.mk

ASFLAGS = -Sc -Sv:ALP

CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE);
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE);

TARGET = wing32

OBJS =   initterm.obj wing32.obj resource.obj

LIBS =  $(PDWIN32_LIB)\user32.lib $(PDWIN32_LIB)\gdi32.lib $(PDWIN32_LIB)\kernel32.lib $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)


all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def $(LIBS)
  $(CP) $@ $(PDWIN32_BIN)

$(TARGET)exp.def: $(TARGET).def
    $(IMPDEF) $** $@

$(TARGET).lib: $(TARGET)exp.Def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $**
    $(CP) $@ $(PDWIN32_LIB)\$@


wing32.obj: wing32.cpp\
    wing32impl.h\
    $(PDWIN32_INCLUDE)\win\wingdi.h \
    $(PDWIN32_INCLUDE)\win\winuser.h


resource.asm: $(TARGET).rc
    $(RC) $(RCFLAGS) -o resource.asm $(TARGET).rc

resource.obj: resource.asm

initterm.obj: initterm.cpp wing32impl.h

clean:
  $(RM) *.obj *.lib *.dll *.map *.pch
  $(RM) $(PDWIN32_BIN)\$(TARGET).dll
  $(RM) $(PDWIN32_LIB)\$(TARGET).lib
