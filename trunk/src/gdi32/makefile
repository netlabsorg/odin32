# $Id: makefile,v 1.20 2000-02-09 23:40:45 bird Exp $

#
# PD-Win32 API
#
#       gdi32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)

TARGET = gdi32

OBJS =  gdi32.obj opengl.obj callback.obj dibsect.obj initterm.obj resource.obj \
        oslibgdi.obj font.obj text.obj palette.obj line.obj oslibgpi.obj region.obj \
        metafile.obj dibitmap.obj blit.obj rgbcvt.obj


all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
    $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
          $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib $(PDWIN32_LIB)/user32.lib \
          $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
    $(CP) $@ $(PDWIN32_BIN)


lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib

$(PDWIN32_LIB)\$(TARGET).lib: $(TARGET).lib
    $(CP) $** $@

$(TARGET).lib: $(TARGET)exp.def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
    $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
    $(IMPDEF) $** $@


resource.asm: gdi32.rc
    $(RC) $(RCFLAGS) -o resource.asm gdi32.rc


dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
    *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.lib *.dll *.map *.pch resource.asm \
        $(PDWIN32_BIN)\$(TARGET).dll $(PDWIN32_LIB)\$(TARGET).lib \
        $(TARGET)exp.def

