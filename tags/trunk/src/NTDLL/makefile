# $Id: makefile,v 1.10 1999-11-03 23:28:04 sandervl Exp $

#
# PD-Win32 API
#
#       NTDLL.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


OBJS = NTDLL.obj\
       file.obj\
       exception.obj\
       nt.obj\
       om.obj\
       reg.obj\
       rtl.obj\
       rtlstr.obj\
       sec.obj\
       sync.obj\
       initterm.obj\
       crt.obj\
       time.obj\
       unknown.obj


TARGET = NTDLL

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) sbsstub.obj $(TARGET).def $(PDWIN32_LIB)\kernel32.lib $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
# PMWINX.LIB LIBULS.LIB LIBCONV.LIB
        $(CP) $@ $(PDWIN32_BIN)

$(TARGET).lib: $(TARGET).def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $**
        $(CP) $@ $(PDWIN32_LIB)



ntdll.obj:  .\ntdll.cpp  .\ntdll.h

file.obj:   .\file.cpp   .\ntdll.h

nt.obj:     .\nt.cpp     .\ntdll.h

om.obj:     .\om.cpp     .\ntdll.h

reg.obj:    .\reg.cpp    .\ntdll.h

rtl.obj:    .\rtl.cpp    .\ntdll.h

rtlstr.obj: .\rtlstr.cpp .\ntdll.h

sec.obj:    .\sec.cpp    .\ntdll.h

sync.obj:   .\sync.cpp   .\ntdll.h

time.obj:   .\time.cpp   .\ntdll.h

initterm.obj: .\initterm.cpp

crt.obj:    .\crt.cpp    .\ntdll.h

unknown.obj: .\unknown.cpp .\ntdll.h

exception.obj: .\exception.cpp .\ntdll.h


clean:
        $(RM) *.obj *.lib *.dll *~ *.map *.pch
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll

