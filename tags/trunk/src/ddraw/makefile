#
# PD-Win32 API
#
# ddraw.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk
ASFLAGS = -Sc -Sv:ALP

CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE);
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE);

TARGET = ddraw

OBJS =  ddraw.obj os2ddraw.obj os2clipper.obj os2d3d.obj iccio1.obj asmutil.obj\
        os2surface.obj os2palette.obj os2palset.obj misc.obj rectangle.obj initterm.obj

LIBS =  ..\..\lib\pmwinx.lib mmpm2.lib ..\..\lib\comctl32.lib $(PDWIN32_LIB)\advapi32.lib $(PDWIN32_LIB)\kernel32.lib $(PDWIN32_LIB)\gdi32.lib

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def $(LIBS)
  $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET).dll
  $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET).dll
  $(CP) $@ $(PDWIN32_LIB)


ddraw.obj: ddraw.cpp  \
    os2ddraw.h        \
    $(PDWIN32_INCLUDE)\win\ddraw.h

os2ddraw.obj: os2ddraw.cpp  \
    os2palette.h \
    os2clipper.h \
    os2surface.h \
    os2ddraw.h\
    $(PDWIN32_INCLUDE)\win\ddraw.h \
    $(PDWIN32_INCLUDE)\win\d3d.h

os2d3d.obj: os2d3d.cpp \
    os2ddraw.h\
    $(PDWIN32_INCLUDE)\win\ddraw.h \
    $(PDWIN32_INCLUDE)\win\d3d.h

misc.obj: misc.cpp

os2palette.obj: os2palette.cpp  \
    os2palette.h \
    os2clipper.h \
    os2surface.h \
    os2palset.h \
    os2ddraw.h

os2palset.obj: os2palset.cpp  \
    os2palset.h

os2clipper.obj: os2clipper.cpp  \
    os2palette.h \
    os2clipper.h \
    os2surface.h \
    os2ddraw.h\
    $(PDWIN32_INCLUDE)\win\ddraw.h \
    $(PDWIN32_INCLUDE)\win\d3d.h

os2surface.obj: os2surface.cpp  \
    os2palette.h \
    os2clipper.h \
    os2surface.h \
    os2ddraw.h\
    $(PDWIN32_INCLUDE)\win\ddraw.h \
    $(PDWIN32_INCLUDE)\win\d3d.h

rectangle.obj: rectangle.cpp

initterm.obj: initterm.cpp

clean:
  $(RM) *.obj *.lib *.dll *~ *.map *.pch
  $(RM) $(PDWIN32_BIN)\$(TARGET).dll
  $(RM) $(PDWIN32_LIB)\$(TARGET).lib
