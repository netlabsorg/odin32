# $Id: makefile,v 1.14 2000-02-05 01:58:50 sandervl Exp $

#
# PD-Win32 API
#
#       dsound.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = dsound

OBJS =  dsound.obj os2dsound.obj initterm.obj os2sndbuffer.obj os2helper.obj resource.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              pmwinx.lib $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET).def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $**
        $(CP) $@ $(PDWIN32_LIB)

resource.asm: $(TARGET).rc
    $(RC) $(RCFLAGS) -o resource.asm $(TARGET).rc

resource.obj: resource.asm


dsound.obj: dsound.cpp \
    $(PDWIN32_INCLUDE)/misc.h \
    os2dsound.h

os2dsound.obj: os2dsound.cpp \
    $(PDWIN32_INCLUDE)/misc.h \
    os2sndbuffer.h \
    os2dsound.h

os2sndbuffer.obj: os2sndbuffer.cpp \
    $(PDWIN32_INCLUDE)/misc.h \
    os2sndbuffer.h \
    os2dsound.h

os2helper.obj: os2helper.cpp\
    $(PDWIN32_INCLUDE)/os2wrap.h

initterm.obj: initterm.cpp initterm.h


clean:
        $(RM) *.obj *.lib *.dll *.map *.pch
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib

