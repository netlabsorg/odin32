# $Id: makefile,v 1.16 2000-02-17 14:09:30 sandervl Exp $

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
        mmio.obj driver.obj playsound.obj joyos2.obj resource.obj dbglocal.obj


all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
    $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
          $(PDWIN32_LIB)/pmwinx.lib mmpm2.lib  \
          $(PDWIN32_LIB)/kernel32.lib \
          $(PDWIN32_LIB)/user32.lib $(PDWIN32_LIB)/odincrt.lib \
          OS2386.LIB \
          $(RTLLIB_O)
    $(CP) $@ $(PDWIN32_BIN)


lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib

$(PDWIN32_LIB)\$(TARGET).lib: $(TARGET).lib
    $(CP) $** $@

$(TARGET).lib: $(TARGET)exp.def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
    $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
    $(IMPDEF) $** $@


resource.asm: $(TARGET).rc
    $(RC) $(RCFLAGS) -o resource.asm $(TARGET).rc


dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.lib *.dll *.map *.pch \
        $(PDWIN32_BIN)\$(TARGET).dll $(PDWIN32_LIB)\$(TARGET).lib \
        resource.asm $(TARGET)exp.def
