#
# PD-Win32 API 
#
#	ole32.dll makefile
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

$(ORGTARGET).lib: $(ORGTARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(ORGTARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(ORGTARGET)exp.def: $(ORGTARGET).def
        $(IMPDEF) $** $@

resource.asm: $(ORGTARGET).rc
    $(RC) $(RCFLAGS) -o resource.asm $(ORGTARGET).rc

resource.obj: resource.asm

.\stubs.obj: \
    .\stubs.cpp \
    .\ole32.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h

.\storage.obj: \
    .\storage.cpp \
    .\ole32.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    $(PDWIN32_INCLUDE)/win/debugtools.h \
    $(PDWIN32_INCLUDE)/heapstring.h \
    .\storage.h \
    $(PDWIN32_INCLUDE)/win/wtypes.h \
    $(PDWIN32_INCLUDE)/win/winnt.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_storage.h

.\stg_stream.obj: \
    .\stg_stream.cpp \
    .\ole32.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    $(PDWIN32_INCLUDE)/win/debugtools.h \
    $(PDWIN32_INCLUDE)/heapstring.h \
    .\storage.h \
    $(PDWIN32_INCLUDE)/win/wtypes.h \
    $(PDWIN32_INCLUDE)/win/winnt.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_storage.h

.\stg_bigblockfile.obj: \
    .\stg_bigblockfile.cpp \
    .\ole32.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    $(PDWIN32_INCLUDE)/win/debugtools.h \
    $(PDWIN32_INCLUDE)/heapstring.h \
    .\storage.h \
    $(PDWIN32_INCLUDE)/win/wtypes.h \
    $(PDWIN32_INCLUDE)/win/winnt.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_storage.h

.\regsvr32.obj: \
    .\regsvr32.cpp

.\oString.obj: \
    .\oString.cpp \
    .\ole32.h \
    .\oString.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h

.\oleobj.obj: \
    .\oleobj.cpp \
    .\ole32.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    $(PDWIN32_INCLUDE)/win/debugtools.h

.\oleMenu.obj: \
    .\oleMenu.cpp \
    .\ole32.h \
    .\oString.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    $(PDWIN32_INCLUDE)/win/commctrl.h

.\oleDrag.obj: \
    .\oleDrag.cpp \
    .\ole32.h \
    .\oString.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    $(PDWIN32_INCLUDE)/win/commctrl.h

.\oleClip.obj: \
    .\oleClip.cpp \
    .\ole32.h \
    .\oString.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    $(PDWIN32_INCLUDE)/win/commctrl.h

.\ole32.obj: \
    .\ole32.cpp \
    .\ole32.h \
    .\oString.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    .\filemoniker.h \
    .\moniker.h

.\ole2.obj: \
    .\ole2.cpp \
    .\ole32.h \
    .\oString.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    $(PDWIN32_INCLUDE)/win/commctrl.h

.\moniker.obj: \
    .\moniker.cpp \
    .\ole32.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    .\moniker.h

.\memlockbytes.obj: \
    .\memlockbytes.cpp \
    .\ole32.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    $(PDWIN32_INCLUDE)/win/debugtools.h

.\library.obj: \
    .\library.cpp \
    .\ole32.h \
    .\oString.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    .\moniker.h

.\iunknown.obj: \
    .\iunknown.cpp \
    .\ole32.h \
    .\oString.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h

.\itemmoniker.obj: \
    .\itemmoniker.cpp \
    .\ole32.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    $(PDWIN32_INCLUDE)/win/debugtools.h \
    $(PDWIN32_INCLUDE)/heapstring.h

.\initialise.obj: \
    .\initialise.cpp \
    .\ole32.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h

.\hglobalstream.obj: \
    .\hglobalstream.cpp \
    .\ole32.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    $(PDWIN32_INCLUDE)/win/debugtools.h \
    $(PDWIN32_INCLUDE)/heapstring.h

.\guid.obj: \
    .\guid.cpp \
    .\ole32.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h

.\filemoniker.obj: \
    .\filemoniker.cpp \
    .\ole32.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    $(PDWIN32_INCLUDE)/win/debugtools.h \
    $(PDWIN32_INCLUDE)/heapstring.h \
    .\filemoniker.h

.\defaulthandler.obj: \
    .\defaulthandler.cpp \
    .\ole32.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    $(PDWIN32_INCLUDE)/win/debugtools.h \
    $(PDWIN32_INCLUDE)/heapstring.h \
    $(PDWIN32_INCLUDE)/win/oleauto.h

.\datacache.obj: \
    .\datacache.cpp \
    .\ole32.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    $(PDWIN32_INCLUDE)/win/debugtools.h \
    $(PDWIN32_INCLUDE)/heapstring.h

.\compositemoniker.obj: \
    .\compositemoniker.cpp \
    .\ole32.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    $(PDWIN32_INCLUDE)/win/debugtools.h

.\clsid.obj: \
    .\clsid.cpp \
    .\ole32.h \
    .\oString.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h

.\bindctx.obj: \
    .\bindctx.cpp \
    .\ole32.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    $(PDWIN32_INCLUDE)/win/debugtools.h \
    $(PDWIN32_INCLUDE)/heapstring.h

.\antimoniker.obj: \
    .\antimoniker.cpp \
    .\ole32.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h \
    $(PDWIN32_INCLUDE)/win/debugtools.h

.\taskmem.obj: \
    .\taskmem.cpp \
    .\ole32.h \
    .\oString.h \
    $(PDWIN32_INCLUDE)/win/objbase.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_inplace.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleobj.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_surrogate.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_oleview.h \
    $(PDWIN32_INCLUDE)/win/wine/obj_cache.h

regsvr32.obj:	regsvr32.cpp
    $(CC) -C $(CXXFLAGS) -Ge+ -Tm+ $**

regsvr32.exe:	regsvr32.obj regsvr32.def \
		$(PDWIN32_LIB)/user32.lib \
		$(PDWIN32_LIB)/kernel32.lib \
		$(PDWIN32_LIB)/ole32.lib
	$(LD) $(LDFLAGS) -Ge+ -Fm -Fe$@ $** \
		$(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
	$(CP) $@ $(PDWIN32_BIN)

clean:
	$(RM) *.obj *.lib *.dll *.map *.pch	
	$(RM) $(PDWIN32_BIN)\$(TARGET).dll
	$(RM) $(PDWIN32_LIB)\$(TARGET).lib
	$(RM) $(PDWIN32_BIN)\regsvr32.exe


