#/* $Id: makefile,v 1.1 2000-01-27 21:49:14 sandervl Exp $ */
#
# PD-Win32 API
#
#       opengl32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE) -DDEBUG
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE) -DDEBUG


TARGET1 = opengl32
TARGET2 = glu32
TARGET3 = glut32

OBJS1=  opengl32.obj initterm.obj
OBJS2=  glu32.obj glucb32.obj initterm.obj
OBJS3=  glut32.obj glucb32.obj initterm.obj

all: $(TARGET1).dll $(TARGET1).lib $(TARGET2).dll $(TARGET2).lib $(TARGET3).dll $(TARGET3).lib

$(TARGET1).dll: $(OBJS1) $(TARGET1).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS1) $(TARGET1).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/user32.lib $(PDWIN32_LIB)/odincrt.lib \
              OS2386.LIB $(RTLLIB_O)
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET1).lib: $(TARGET1)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET1)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET1)exp.def: $(TARGET1).def
        $(IMPDEF) $** $@

$(TARGET2).dll: $(OBJS2) $(TARGET2).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS2) $(TARGET2).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/user32.lib $(PDWIN32_LIB)/odincrt.lib \
              OS2386.LIB $(RTLLIB_O)
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET2).lib: $(TARGET2)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET2)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET2)exp.def: $(TARGET2).def
        $(IMPDEF) $** $@


$(TARGET3).dll: $(OBJS3) $(TARGET3).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS3) $(TARGET3).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/user32.lib $(PDWIN32_LIB)/odincrt.lib \
              OS2386.LIB $(RTLLIB_O)
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET3).lib: $(TARGET3)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET3)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET3)exp.def: $(TARGET3).def
        $(IMPDEF) $** $@


opengl32.obj: opengl32.cpp pglos2.h glos2.h initterm.h
glu32.obj:    glu32.cpp glos2.h gluos2.h initterm.h glucb32.h
glut32.obj:   glut32.cpp glos2.h gluos2.h glutos2.h initterm.h glucb32.h
initterm.obj: initterm.cpp initterm.h
glucb32.obj:  glucb32.cpp glucb32.h glos2.h gluos2.h

clean:
        $(RM) *.obj *.lib *.dll *.map *.pch
        $(RM) $(PDWIN32_BIN)\$(TARGET1).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET1).lib
        $(RM) $(PDWIN32_BIN)\$(TARGET2).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET2).lib
        $(RM) $(PDWIN32_BIN)\$(TARGET3).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET3).lib

