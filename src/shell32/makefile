# $Id: makefile,v 1.11 1999-10-04 09:55:59 sandervl Exp $

#
# PD-Win32 API
#
#       shell32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = shell32

OBJS =  shell32.obj sh.obj she.obj drag.obj initterm.obj \
        misc.obj ordinal.obj unknown.obj path.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/user32.lib $(PDWIN32_LIB)/advapi32.lib \
              $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@

shell32.obj: shell32.cpp shell32.h

drag.obj: drag.cpp shell32.h

sh.obj: sh.cpp shell32.h

she.obj: she.cpp shell32.h

misc.obj: misc.cpp shell32.h

ordinal.obj: ordinal.cpp shell32.h

unknown.obj: unknown.cpp shell32.h

path.obj: path.cpp shell32.h

initterm.obj: initterm.cpp


clean:
        $(RM) *.obj *.lib *.dll *~ *.map *.pch
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib



