# $Id: makefile,v 1.3 2000-12-02 23:43:28 bird Exp $

#
# Odin32 API
#
#       wsock32.dll makefile
#

OLD_STYLE=1
!include ../../../makefile.inc


CFLAGS = $(CFLAGS) -I$(ODIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(ODIN32_INCLUDE)


TARGET = wsock32

OBJS =  wsock32.obj initterm.obj unknown.obj async.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
    $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(ODIN32_LIB)\pmwinx.lib $(ODIN32_LIB)\user32.lib \
              tcp32dll.lib so32dll.lib  $(ODIN32_LIB)\kernel32.lib \
              $(ODIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
    $(CP) $@ $(ODIN32_BIN)

$(TARGET).lib: $(TARGET)exp.def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
    $(CP) $@ $(ODIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
    $(IMPDEF) $** $@

wsock32.obj: wsock32.cpp wsock32.h wsock32const.h
unknown.obj: unknown.cpp wsock32.h
async.obj:   async.cpp wsock32const.h
initterm.obj: initterm.cpp


clean:
    $(RM) *.obj *.lib *.dll *~ *.map *.pch
    $(RM) $(ODIN32_BIN)\$(TARGET).dll
    $(RM) $(ODIN32_LIB)\$(TARGET).lib

