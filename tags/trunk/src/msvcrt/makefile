# $Id: makefile,v 1.12 2000-02-09 23:46:58 bird Exp $

#
# PD-Win32 API
#
#       msvcrt.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)

RC = $(PDWIN32_TOOLS)\wrc
RCFLAGS = -s -I. -I$(CPPMAIN)\include -I$(PDWIN32_INCLUDE) -I$(PDWIN32_INCLUDE)\win

TARGET = msvcrt
TARGET20 = msvcrt20
TARGET40 = msvcrt40

OBJS   =  msvcrt.obj resource.obj $(PDWIN32_LIB)/dllentry.obj
OBJS20 =  msvcrt.obj resource20.obj $(PDWIN32_LIB)/dllentry.obj
OBJS40 =  msvcrt.obj resource40.obj $(PDWIN32_LIB)/dllentry.obj

all: $(TARGET).dll $(TARGET).lib $(TARGET20).dll $(TARGET20).lib \
     $(TARGET40).dll $(TARGET40).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
    $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
          $(PDWIN32_LIB)/kernel32.lib \
          $(PDWIN32_LIB)/crtdll.lib  \
          $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
    $(CP) $@ $(PDWIN32_BIN)

$(TARGET20).dll: $(OBJS20) $(TARGET20).def
    $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS20) $(TARGET20).def \
          $(PDWIN32_LIB)/kernel32.lib \
          $(PDWIN32_LIB)/crtdll.lib \
          $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
    $(CP) $@ $(PDWIN32_BIN)

$(TARGET40).dll: $(OBJS40) $(TARGET40).def
    $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS40) $(TARGET40).def \
          $(PDWIN32_LIB)/kernel32.lib \
          $(PDWIN32_LIB)/crtdll.lib  \
          $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
    $(CP) $@ $(PDWIN32_BIN)


lib: $(TARGET).lib  $(PDWIN32_LIB)\$(TARGET).lib \
                    $(PDWIN32_LIB)\$(TARGET20).lib \
                    $(PDWIN32_LIB)\$(TARGET40).lib

$(PDWIN32_LIB)\$(TARGET).lib: $(TARGET).lib
    $(CP) $** $@

$(PDWIN32_LIB)\$(TARGET20).lib: $(TARGET20).lib
    $(CP) $** $@

$(PDWIN32_LIB)\$(TARGET40).lib: $(TARGET40).lib
    $(CP) $** $@

$(TARGET).lib: $(TARGET).def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $**
    $(CP) $@ $(PDWIN32_LIB)

$(TARGET20).lib: $(TARGET20).def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $**
    $(CP) $@ $(PDWIN32_LIB)

$(TARGET40).lib: $(TARGET40).def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $**
    $(CP) $@ $(PDWIN32_LIB)

resource.asm: msvcrt.rc
    $(RC) $(RCFLAGS) -o resource.asm msvcrt.rc

resource20.asm: msvcrt20.rc
    $(RC) $(RCFLAGS) -o resource20.asm msvcrt20.rc

resource40.asm: msvcrt40.rc
    $(RC) $(RCFLAGS) -o resource40.asm msvcrt40.rc


dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.lib *.dll *.map *.pch resource*.asm \
        $(PDWIN32_BIN)\$(TARGET).dll \
        $(PDWIN32_BIN)\$(TARGET20).dll \
        $(PDWIN32_BIN)\$(TARGET40).dll \
        $(PDWIN32_LIB)\$(TARGET).lib
        $(PDWIN32_LIB)\$(TARGET20).lib
        $(PDWIN32_LIB)\$(TARGET40).lib

