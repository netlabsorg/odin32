# $Id: makefile,v 1.7 1999-08-21 12:29:31 sandervl Exp $

#
# PD-Win32 API
#
#       pe.exe makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) /Ge+ -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) /Ge+ -I$(PDWIN32_INCLUDE)
LDFLAGSEXE = $(LDFLAGS) /Ge+ /B"/pmtype:pm /stack:0x30000 /NOBASE /Map" \
             $(PDWIN32_LIB)\peldr.lib os2386.lib $(PDWIN32_LIB)\odincrt.lib $(RTLLIB_O)


TARGET = pe
TARGETDLL = peldr

OBJS =  pe.obj
OBJSDLL =  initterm.obj

all: $(TARGETDLL).dll $(TARGETDLL).lib $(TARGET).exe


$(TARGET).exe: $(OBJS)
        $(LD) $(LDFLAGSEXE) -Fe$@ $(OBJS)
        $(CP) $(TARGET).exe $(PDWIN32_BIN)

$(TARGETDLL).dll: $(OBJSDLL) $(TARGETDLL).def 
	$(LD) $(LDFLAGS) -Fe$@ $(OBJSDLL) $(TARGETDLL).def OS2386.LIB $(RTLLIB_O)
	$(CP) $@ $(PDWIN32_BIN)

$(TARGETDLL).lib: $(TARGETDLL).dll
	$(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGETDLL).dll
	$(CP) $@ $(PDWIN32_LIB)


initterm.obj: initterm.cpp
misc.obj: misc.cpp
pe.obj: pe.cpp pe.h $(PDWIN32_INCLUDE)\winimage.h $(PDWIN32_INCLUDE)\winexe.h

clean:
        $(RM) *.obj *.lib *.dll *~ *.map *.pch *.exe
        $(RM) $(PDWIN32_BIN)\$(TARGET).exe
	$(RM) $(PDWIN32_BIN)\$(TARGETDLL).dll
	$(RM) $(PDWIN32_LIB)\$(TARGETDLL).lib
