# $Id: makefile,v 1.2 1999-06-01 21:52:15 phaller Exp $

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


OBJS = NTDLL.obj   \
       file.obj    \
       nt.obj      \
       om.obj      \
       reg.obj     \
       rtl.obj     \
       rtlstr.obj  \
       sec.obj     \
       sync.obj    \
       time.obj


TARGET = NTDLL

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def $(PDWIN32_LIB)\kernel32.lib
# PMWINX.LIB LIBULS.LIB LIBCONV.LIB
        $(CP) $@ $(PDWIN32_BIN)

$(TARGET).lib: $(TARGET).dll
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET).dll
        $(CP) $@ $(PDWIN32_LIB)



ntdll.obj:  .\ntdll.c  .\ntdll.h

file.obj:   .\file.c   .\ntdll.h

nt.obj:     .\nt.c     .\ntdll.h

om.obj:     .\om.c     .\ntdll.h

reg.obj:    .\reg.c    .\ntdll.h

rtl.obj:    .\rtl.c    .\ntdll.h

rtlstr.obj: .\rtlstr.c .\ntdll.h

sec.obj:    .\sec.c    .\ntdll.h

sync.obj:   .\sync.c   .\ntdll.h

time.obj:   .\time.c   .\ntdll.h



clean:
        $(RM) *.obj *.lib *.dll *~ *.map *.pch
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll

