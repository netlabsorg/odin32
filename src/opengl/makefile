# $Id: makefile,v 1.11 2000-02-25 00:57:10 sandervl Exp $
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


!ifdef DEBUG
!ifdef NODEBUGINFO
MAKE_CMD=nmake DEBUG=1 NODEBUGINFO=1
!else
MAKE_CMD=nmake DEBUG=1
!endif
!else
MAKE_CMD=nmake
!endif

CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)
!ifdef NODEBUGINFO
LDFLAGS=$(LDFLAGS) /NODEBUG
!endif

TARGET1  = opengl32
DTARGET1 = opengl32
TARGET2  = glu\glu32
DTARGET2 = glu32
TARGET3  = glut\glut32
DTARGET3 = glut32
TARGET4  = glide
DTARGET4 = glide2x

OBJS1=  opengl32.obj initterm.obj resource.obj

all: $(TARGET1).dll $(TARGET1).lib \
     $(TARGET2).dll $(TARGET2).lib \
     $(TARGET3).dll $(TARGET3).lib $(TARGET4)

$(TARGET1).dll: $(OBJS1) $(TARGET1).def
    $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS1) $(TARGET1).def \
          $(PDWIN32_LIB)/gdi32.lib $(PDWIN32_LIB)/kernel32.lib \
          $(PDWIN32_LIB)/user32.lib $(PDWIN32_LIB)/odincrt.lib \
          OS2386.LIB $(RTLLIB_O)
    $(CP) $@ $(PDWIN32_BIN)


$(TARGET1).lib: $(TARGET1)exp.def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET1)exp.def
    $(CP) $@ $(PDWIN32_LIB)

$(TARGET1)exp.def: $(TARGET1).def
    $(IMPDEF) $** $@


resource.asm: $(TARGET1).rc
    $(RC) $(RCFLAGS) -o resource.asm $(TARGET1).rc


$(TARGET2).dll:
    cd glu
    $(MAKE_CMD)
    cd ..

$(TARGET3).dll:
    cd glut
    $(MAKE_CMD)
    cd ..


$(TARGET2).lib:
    cd glu
    $(MAKE_CMD) $(@F)
    cd ..


$(TARGET3).lib:
    cd glut
    $(MAKE_CMD) $(@F)
    cd ..


$(TARGET4):
    cd glide
    $(MAKE_CMD)
    cd ..

lib: $(TARGET1).lib $(PDWIN32_LIB)\$(DTARGET1).lib \
     $(TARGET2).lib $(PDWIN32_LIB)\$(DTARGET2).lib \
     $(TARGET3).lib $(PDWIN32_LIB)\$(DTARGET3).lib \
     $(PDWIN32_LIB)\$(DTARGET4).lib

$(PDWIN32_LIB)\$(DTARGET1).lib: $(TARGET1).lib
    $(CP) $** $@

$(PDWIN32_LIB)\$(DTARGET2).lib: $(TARGET2).lib
    $(CP) $** $@

$(PDWIN32_LIB)\$(DTARGET3).lib: $(TARGET3).lib
    $(CP) $** $@

$(PDWIN32_LIB)\$(DTARGET4).lib:
    cd glide
    $(MAKE_CMD) lib
    cd ..

dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h
    cd glut
    $(MAKE_CMD) NODEP=1 dep
    cd ..\glu
    $(MAKE_CMD) NODEP=1 dep
    cd ..\glide
    $(MAKE_CMD) NODEP=1 dep
    cd ..

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.lib *.dll *.map resource.asm *exp.def \
        $(RM) $(PDWIN32_BIN)\$(TARGET1).dll $(PDWIN32_LIB)\$(TARGET1).lib
    cd glut
    $(MAKE_CMD) clean
    cd ..\glu
    $(MAKE_CMD) clean
    cd ..\glide
    $(MAKE_CMD) clean
    cd ..

