# $Id: makefile,v 1.5 1999-12-21 12:27:12 sandervl Exp $

#
# PD-Win32 API
#
#	crtdll.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE) 
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = crtdll

OBJS =  crtdll.obj bsearch.obj initterm.obj qsort.obj crt.obj asmhlp.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
	$(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib \
	       $(PDWIN32_LIB)/odincrt.lib $(PDWIN32_LIB)/user32.lib OS2386.LIB $(RTLLIB_O) 
	$(CP) $@ $(PDWIN32_BIN)


$(TARGET).LIB: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@


initterm.obj: initterm.cpp
crtdll.obj: crtdll.cpp
bsearch.obj: bsearch.c
qsort.obj:    qsort.c
crt.obj:	crt.cpp
asmhlp.obj: asmhlp.asm

clean:
	$(RM) *.obj *.lib *.dll *.map *.pch
	$(RM) $(PDWIN32_BIN)\$(TARGET).dll
	$(RM) $(PDWIN32_LIB)\$(TARGET).lib





