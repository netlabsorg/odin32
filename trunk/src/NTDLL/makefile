# $Id: makefile,v 1.19 2000-02-09 23:40:41 bird Exp $

#
# PD-Win32 API
#
#       NTDLL.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


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
       time.obj\
       unknown.obj\
       arith64.obj\
       regfunc.obj resource.obj


TARGET = NTDLL

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
    $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) sbsstub.obj $(TARGET).def $(PDWIN32_LIB)\kernel32.lib $(PDWIN32_LIB)\user32.lib $(PDWIN32_LIB)/kernel32.lib $(PDWIN32_LIB)/crtdll.lib $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
    $(CP) $@ $(PDWIN32_BIN)


lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib

$(PDWIN32_LIB)\$(TARGET).lib: $(TARGET).lib
    $(CP) $** $@

$(TARGET).lib: $(TARGET)exp.def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
    $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
    $(IMPDEF) $** $@


resource.asm: $(TARGET).rc
    $(RC) $(RCFLAGS) -o resource.asm $(TARGET).rc


dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
    *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.lib *.dll *.map *.pch \
        $(PDWIN32_LIB)\$(TARGET).lib $(PDWIN32_BIN)\$(TARGET).dll \
        resource.asm $(TARGET)exp.def

