# $Id: makefile,v 1.13 2000-02-04 19:29:17 hugh Exp $

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
ASFLAGS = -Sc -Sv:ALP

CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = dsound

OBJS =  dsound.obj os2dsound.obj initterm.obj os2sndbuffer.obj os2helper.obj asmutil.obj
#OBJS =  dsound.obj initterm.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              pmwinx.lib  $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@


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

asmutil.obj: asmutil.asm

initterm.obj: initterm.cpp initterm.h


clean:
        $(RM) *.obj *.lib *.dll *.map *.pch
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib

