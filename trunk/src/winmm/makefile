# $Id: makefile,v 1.5 1999-08-09 18:58:05 achimha Exp $

#
# PD-Win32 API
#
#       winmm.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = winmm

OBJS =  winmm.obj os2timer.obj waveout.obj dwaveout.obj timegettime.obj \
        wavein.obj aux.obj auxos2.obj mixer.obj \
        midi.obj irtmidi.obj midistrm.obj initterm.obj mci.obj joy.obj \
        mmio.obj driver.obj playsound.obj joyos2.obj


all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) sbsstub.obj $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib mmpm2.lib  \
              $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/user32.lib $(PDWIN32_LIB)/odincrt.lib \
              OS2386.LIB
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET).dll
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET).dll
        $(CP) $@ $(PDWIN32_LIB)


driver.obj:     driver.cpp
winmm.obj:      winmm.cpp
os2timer.obj:   os2timer.h wintimer.h
waveout.obj:    waveout.cpp  dwaveout.h
wavein.obj:     wavein.cpp
mixer.obj:      mixer.cpp  midi.hpp
mci.obj:        mci.cpp
joy.obj:        joy.cpp joyos2.h
mmio.obj:       mmio.cpp
aux.obj:        aux.cpp  aux.h
auxos2.obj:     auxos2.cpp aux.h
midi.obj:       midi.cpp midi.hpp  irtmidi.hpp
midistrm.obj:   midistrm.cpp midi.hpp   irtmidi.hpp
irtmidi.obj:    irtmidi.cpp irtmidi.hpp
dwaveout.obj:   dwaveout.cpp dwaveout.h $(PDWIN32_INCLUDE)\vmutex.h
timegettime.obj: timegettime.cpp
initterm.obj:   initterm.cpp aux.h
playsound.obj:  playsound.cpp
joyos2.obj:     joyos2.cpp joyos2.h

clean:
        $(RM) *.obj *.lib *.dll *~ *.map *.pch
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib

