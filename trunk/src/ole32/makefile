# $Id: makefile,v 1.20 2000-02-10 00:40:06 bird Exp $

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


TARGET = ole32os2
ORGTARGET = ole32

OBJS =  ole32.obj \
        antimoniker.obj \
        bindctx.obj \
        clsid.obj \
        compositemoniker.obj \
        datacache.obj \
        defaulthandler.obj \
        filemoniker.obj \
        guid.obj \
        hglobalstream.obj \
        initialise.obj \
        itemmoniker.obj \
        iunknown.obj \
        library.obj \
        memlockbytes.obj \
        moniker.obj \
        oString.obj \
        ole2.obj \
        oleobj.obj \
        oleClip.obj \
        oleDrag.obj \
        oleMenu.obj \
        stg_bigblockfile.obj \
        stg_stream.obj \
        storage.obj \
        stubs.obj \
        taskmem.obj resource.obj  $(PDWIN32_LIB)/dllentry.obj

all: $(TARGET).dll $(ORGTARGET).lib regsvr32.exe


$(TARGET).dll: $(OBJS) $(ORGTARGET).def
    $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(ORGTARGET).def \
          $(PDWIN32_LIB)\user32.lib \
          $(PDWIN32_LIB)\gdi32.lib \
          $(PDWIN32_LIB)\comctl32.lib \
          $(PDWIN32_LIB)\advapi32.lib \
          $(PDWIN32_LIB)\kernel32.lib \
          $(PDWIN32_LIB)\oleaut32.lib \
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


regsvr32.obj:   regsvr32.cpp
    $(CC) -C $(CXXFLAGS) -Ge+ -Tm+ $**

regsvr32.exe:   regsvr32.obj regsvr32.def \
                $(PDWIN32_LIB)/user32.lib \
                $(PDWIN32_LIB)/kernel32.lib \
                $(PDWIN32_LIB)/ole32.lib
    $(LD) $(LDFLAGS) -Ge+ -Fm -Fe$@ $** \
            $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
    $(CP) $@ $(PDWIN32_BIN)


dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.lib *.dll *.map *.pch *.exe \
        $(PDWIN32_BIN)\$(TARGET).dll $(PDWIN32_LIB)\$(TARGET).lib \
        $(PDWIN32_BIN)\regsvr32.exe resource.asm $(TARGET)exp.def

