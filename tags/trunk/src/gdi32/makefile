# $Id: makefile,v 1.5 1999-08-16 16:28:02 sandervl Exp $

#
# PD-Win32 API
#
#	gdi32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = gdi32

OBJS =  gdi32.obj opengl.obj callback.obj dibsect.obj initterm.obj


all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
	$(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/odincrt.lib OS2386.LIB cppom30O.lib
	$(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET)exp.def
	$(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
	$(CP) $@ $(PDWIN32_LIB)



gdi32.obj: gdi32.cpp \
    callback.h \
    dibsect.h \
    $(PDWIN32_INCLUDE)\misc.h \

dibsect.obj: dibsect.cpp \
    dibsect.h \
    $(PDWIN32_INCLUDE)\misc.h \

opengl.obj: opengl.cpp \
    $(PDWIN32_INCLUDE)\misc.h \

callback.obj: callback.cpp \
    callback.h \
    $(PDWIN32_INCLUDE)\misc.h \

initterm.obj: initterm.cpp


clean:
	$(RM) *.obj *.lib *.dll *~ *.map *.pch	
	$(RM) $(PDWIN32_BIN)\$(TARGET).dll
	$(RM) $(PDWIN32_LIB)\$(TARGET).lib


