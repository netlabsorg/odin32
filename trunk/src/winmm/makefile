# $Id: makefile,v 1.13 2000-02-05 02:16:21 sandervl Exp $

#
# PD-Win32 API
#
#       winmm.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = winmm

OBJS =  os2timer.obj waveout.obj dwaveout.obj time.obj \
        wavein.obj auxiliary.obj auxos2.obj mixer.obj \
        midi.obj irtmidi.obj midistrm.obj initterm.obj mci.obj joy.obj \
        mmio.obj driver.obj playsound.obj joyos2.obj resource.obj


all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib mmpm2.lib  \
              $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/user32.lib $(PDWIN32_LIB)/odincrt.lib \
              OS2386.LIB \
              $(RTLLIB_O)
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET).LIB: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@


resource.asm: $(TARGET).rc
    $(RC) $(RCFLAGS) -o resource.asm $(TARGET).rc

resource.obj: resource.asm

driver.obj:     driver.cpp
os2timer.obj:   os2timer.cpp os2timer.h time.h
waveout.obj:    waveout.cpp  dwaveout.h
wavein.obj:     wavein.cpp
mixer.obj:      mixer.cpp  midi.hpp
mci.obj:        mci.cpp
joy.obj:        joy.cpp joyos2.h
mmio.obj:       mmio.cpp
auxiliary.obj:  auxiliary.cpp  auxiliary.h
auxos2.obj:     auxos2.cpp auxiliary.h
midi.obj:       midi.cpp midi.hpp  irtmidi.hpp
midistrm.obj:   midistrm.cpp midi.hpp   irtmidi.hpp
irtmidi.obj:    irtmidi.cpp irtmidi.hpp
dwaveout.obj:   dwaveout.cpp dwaveout.h $(PDWIN32_INCLUDE)\vmutex.h
time.obj:       time.cpp
initterm.obj:   initterm.cpp auxiliary.h
playsound.obj:  playsound.cpp
joyos2.obj:     joyos2.cpp joyos2.h

clean:
        $(RM) *.obj *.lib *.dll *.map *.pch
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib

