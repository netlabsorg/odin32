# $Id: makefile,v 1.10 2000-02-09 23:40:56 bird Exp $

#
# PD-Win32 API
#
#       wnetap32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = wnetap32
ORGTARGET = netapi32

OBJS =  wnetap32.obj resource.obj $(PDWIN32_LIB)/dllentry.obj

all: $(TARGET).dll $(ORGTARGET).lib


$(TARGET).dll: $(OBJS) $(ORGTARGET).def
    $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(ORGTARGET).def \
          $(PDWIN32_LIB)/kernel32.lib \
          $(PDWIN32_LIB)/user32.lib $(PDWIN32_LIB)/odincrt.lib \
          OS2386.LIB $(RTLLIB_O)
    $(CP) $@ $(PDWIN32_BIN)


lib: $(ORGTARGET).lib $(PDWIN32_LIB)\$(ORGTARGET).lib

$(PDWIN32_LIB)\$(ORGTARGET).lib: $(ORGTARGET).lib
    $(CP) $** $@

$(ORGTARGET).lib: $(ORGTARGET)exp.def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $**
    $(CP) $@ $(PDWIN32_LIB)

$(ORGTARGET)exp.def: $(ORGTARGET).def
    $(IMPDEF) $** $@


resource.asm: $(ORGTARGET).rc
    $(RC) $(RCFLAGS) -o resource.asm $(ORGTARGET).rc


dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h

!ifndef NODEP
!include .depend
!endif


clean:
        $(RM) *.obj *.lib *.dll *~ *.map *.pch \
            $(PDWIN32_BIN)\$(TARGET).dll $(PDWIN32_LIB)\$(TARGET).lib \
            resource.asm $(ORGTARGET)exp.def

