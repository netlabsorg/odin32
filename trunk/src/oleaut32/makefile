# $Id: makefile,v 1.15 2000-02-09 23:40:48 bird Exp $
#
# PD-Win32 API
#
#       ole32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)

!ifdef NODEBUGINFO
CFLAGS = $(CFLAGS) -DDEFAULT_LOGGING_OFF
CXXFLAGS = $(CXXFLAGS) -DDEFAULT_LOGGING_OFF
!endif

TARGET = olautos2
ORGTARGET = oleaut32

OBJS =  oleaut32.obj guid.obj \
        oList.obj \
        initterm.obj olefont.obj \
        variant.obj \
        ole2disp.obj parsedt.obj \
        iPicture.obj \
        iPictureNone.obj \
        iPictureBmp.obj \
        iPictureMeta.obj \
        iPictureIcon.obj \
        iPictureEmf.obj \
        typelib.obj \
        itypelib.obj \
        itypeinfo.obj \
        hash.obj \
        safearray.obj stubs.obj resource.obj

all: $(TARGET).dll $(ORGTARGET).lib


$(TARGET).dll: $(OBJS) $(ORGTARGET).def
    $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(ORGTARGET).def \
          $(PDWIN32_LIB)\pmwinx.lib \
          $(PDWIN32_LIB)\advapi32.lib \
          $(PDWIN32_LIB)\kernel32.lib \
          $(PDWIN32_LIB)\user32.lib \
          $(PDWIN32_LIB)\ole32.lib \
          $(PDWIN32_LIB)\gdi32.lib \
          $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
    $(CP) $@ $(PDWIN32_BIN)



lib: $(ORGTARGET).lib $(PDWIN32_LIB)\$(ORGTARGET).lib

$(PDWIN32_LIB)\$(ORGTARGET).lib: $(ORGTARGET).lib
    $(CP) $** $@

$(ORGTARGET).lib: $(ORGTARGET)exp.def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $(ORGTARGET)exp.def
    $(CP) $@ $(PDWIN32_LIB)

$(ORGTARGET)exp.def: $(ORGTARGET).def
    $(IMPDEF) $** $@


resource.asm: $(ORGTARGET).rc
    $(RC) $(RCFLAGS) -o resource.asm $(ORGTARGET).rc


dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
    *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.lib *.dll *.map *.pch \
        $(PDWIN32_BIN)\$(TARGET).dll $(PDWIN32_LIB)\$(ORGTARGET).lib \
        resource.asm $(ORGTARGET)exp.def

