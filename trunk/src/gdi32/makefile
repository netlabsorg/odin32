# $Id: makefile,v 1.16 2000-02-01 12:53:30 sandervl Exp $

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

RC = $(PDWIN32_TOOLS)\wrc
RCFLAGS = -s -I. -I$(CPPMAIN)\include -I$(PDWIN32_INCLUDE) -I$(PDWIN32_INCLUDE)\win


TARGET = gdi32

OBJS =  gdi32.obj opengl.obj callback.obj dibsect.obj initterm.obj resource.obj \
        oslibgdi.obj font.obj text.obj palette.obj line.obj oslibgpi.obj region.obj \
        metafile.obj dibitmap.obj blit.obj 


all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib $(PDWIN32_LIB)/user32.lib \
              $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
    $(IMPDEF) $** $@

resource.asm: gdi32.rc
    $(RC) $(RCFLAGS) -o resource.asm gdi32.rc

resource.obj: resource.asm


gdi32.obj: gdi32.cpp \
    callback.h \
    dibsect.h \
    $(PDWIN32_INCLUDE)\misc.h \

region.obj: region.cpp $(PDWIN32_INCLUDE)\misc.h

metafile.obj: metafile.cpp $(PDWIN32_INCLUDE)\misc.h

dibitmap.obj: dibitmap.cpp dibsect.h $(PDWIN32_INCLUDE)\misc.h
blit.obj: blit.cpp dibsect.h $(PDWIN32_INCLUDE)\misc.h

font.obj: font.cpp \
    $(PDWIN32_INCLUDE)\misc.h \

text.obj: text.cpp \
    $(PDWIN32_INCLUDE)\misc.h \

palette.obj: palette.cpp \
    $(PDWIN32_INCLUDE)\misc.h dibsect.h \

dibsect.obj: dibsect.cpp \
    dibsect.h \
    $(PDWIN32_INCLUDE)\misc.h \

opengl.obj: opengl.cpp \
    $(PDWIN32_INCLUDE)\misc.h \

callback.obj: callback.cpp \
    callback.h \
    $(PDWIN32_INCLUDE)\misc.h \

oslibgdi.obj: oslibgdi.cpp oslibgdi.h

line.obj: line.cpp \
    $(PDWIN32_INCLUDE)\misc.h \

oslibgpi.obj: oslibgpi.cpp oslibgpi.h

initterm.obj: initterm.cpp


clean:
        $(RM) *.obj *.lib *.dll *.map *.pch resource.asm
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib
    $(RM) $(TARGET)exp.def

