# $Id: makefile,v 1.9 1999-10-20 22:36:54 sandervl Exp $

#
# PD-Win32 API
#
#	gdi32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = gdi32

OBJS =  gdi32.obj opengl.obj callback.obj dibsect.obj initterm.obj oslibgdi.obj font.obj


all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
	$(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
	$(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET)exp.def
	$(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
	$(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
    $(IMPDEF) $** $@



gdi32.obj: gdi32.cpp \
    callback.h \
    dibsect.h \
    $(PDWIN32_INCLUDE)\misc.h \

font.obj: font.cpp \
    $(PDWIN32_INCLUDE)\misc.h \

dibsect.obj: dibsect.cpp \
    dibsect.h \
    $(PDWIN32_INCLUDE)\misc.h \

opengl.obj: opengl.cpp \
    $(PDWIN32_INCLUDE)\misc.h \

callback.obj: callback.cpp \
    callback.h \
    $(PDWIN32_INCLUDE)\misc.h \

oslibgdi.obj: oslibgdi.cpp oslibgdi.h

initterm.obj: initterm.cpp


clean:
	$(RM) *.obj *.lib *.dll *~ *.map *.pch
	$(RM) $(PDWIN32_BIN)\$(TARGET).dll
	$(RM) $(PDWIN32_LIB)\$(TARGET).lib
    $(RM) $(TARGET)exp.def

