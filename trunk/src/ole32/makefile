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


TARGET = ole32

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
	initterm.obj \
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
	taskmem.obj

all: $(TARGET).dll $(TARGET).lib regsvr32.exe


$(TARGET).dll: $(OBJS) $(TARGET).def 
	$(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)\pmwinx.lib \
              $(PDWIN32_LIB)\user32.lib \
              $(PDWIN32_LIB)\gdi32.lib \
              $(PDWIN32_LIB)\comctl32.lib \
              $(PDWIN32_LIB)\advapi32.lib \
              $(PDWIN32_LIB)\kernel32.lib \
              $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
	$(CP) $@ $(PDWIN32_BIN)
#              $(PDWIN32_LIB)\ntdll.lib \

$(TARGET).lib: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@


antimoniker.obj:	antimoniker.cpp
bindctx.obj:		bindctx.cpp
clsid.obj:		clsid.cpp 
compositemoniker.obj:	compositemoniker.cpp
datacache.obj:		datacache.cpp
defaulthandler.obj:	defaulthandler.cpp
filemoniker.obj:	filemoniker.cpp
guid.obj:		guid.cpp 
hglobalstream.obj:	hglobalstream.cpp
initialise.obj:		initialise.cpp 
initterm.obj:		initterm.cpp
iunknown.obj:		iunknown.cpp
itemmoniker.obj:	itemmoniker.cpp
library.obj:		library.cpp
memlockbytes.obj:	memlockbytes.cpp
moniker.obj:		moniker.cpp
oString.obj:		oString.cpp 
ole2.obj:		ole2.cpp 
oleobj.obj:		oleobj.cpp 
ole32.obj:		ole32.cpp 
oleClip.obj:		oleClip.cpp 
oleDrag.obj:		oleDrag.cpp 
oleMenu.obj:		oleMenu.cpp 
stg_bigblockfile.obj:	stg_bigblockfile.cpp
stg_stream.obj:		stg_stream.cpp
storage.obj:		storage.cpp
stubs.obj:		stubs.cpp 
taskmem.obj:		taskmem.cpp

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
	$(RM) *.obj *.lib *.dll *~ *.map *.pch	
	$(RM) $(PDWIN32_BIN)\$(TARGET).dll
	$(RM) $(PDWIN32_LIB)\$(TARGET).lib
	$(RM) $(PDWIN32_BIN)\regsvr32.exe


