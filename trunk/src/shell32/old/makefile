# $Id: makefile,v 1.2 2000-12-02 23:57:22 bird Exp $

#
# Odin32 API
#
#       shell32.dll makefile
#

!error "don't work?"

OLD_STYLE=1
!include ../../../makefile.inc



TARGET = shell32

OBJS =  shell32.obj sh.obj she.obj drag.obj initterm.obj \
        misc.obj ordinal.obj unknown.obj path.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(ODIN32_LIB)/pmwinx.lib $(ODIN32_LIB)/kernel32.lib \
              $(ODIN32_LIB)/user32.lib $(ODIN32_LIB)/advapi32.lib \
              $(ODIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
        $(CP) $@ $(ODIN32_BIN)


$(TARGET).lib: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(ODIN32_LIB)

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
        $(RM) $(ODIN32_BIN)\$(TARGET).dll
        $(RM) $(ODIN32_LIB)\$(TARGET).lib



