# $Id: makefile,v 1.14 2000-02-09 23:40:42 bird Exp $

#
# PD-Win32 API
#
#       comdlg32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)

TARGET = comdlg32

OBJS =  comdlg32.obj initterm.obj resource.obj cdlg32.obj finddlg32.obj filedlg95.obj \
        filedlgbrowser.obj filetitle.obj comdlgguid.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
    $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
          $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib \
          $(PDWIN32_LIB)/user32.lib $(PDWIN32_LIB)/odincrt.lib $(PDWIN32_LIB)/shlwapi.lib \
          $(PDWIN32_LIB)/shell32.lib $(PDWIN32_LIB)/gdi32.lib OS2386.LIB $(RTLLIB_O)
    $(CP) $@ $(PDWIN32_BIN)


lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib

$(PDWIN32_LIB)\$(TARGET).lib: $(TARGET).lib
    $(CP) $** $@

$(TARGET).lib: $(TARGET)exp.def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
    $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
    $(IMPDEF) $** $@

resource.asm: rsrc.rc
    $(RC) $(RCFLAGS) -o resource.asm rsrc.rc


dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
    *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.lib *.dll *.map *.pch \
        $(PDWIN32_BIN)\$(TARGET).dll $(PDWIN32_LIB)\$(TARGET).lib \
        resource.asm $(TARGET)exp.def
