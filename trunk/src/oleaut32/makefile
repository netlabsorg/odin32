# $Id: makefile,v 1.13 2000-01-26 23:48:29 sandervl Exp $
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
	safearray.obj stubs.obj

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


$(ORGTARGET).lib: $(ORGTARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(ORGTARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(ORGTARGET)exp.def: $(ORGTARGET).def
        $(IMPDEF) $** $@

hash.obj:		hash.cpp oList.h
oList.obj:		oList.cpp oList.h
oleaut32.obj:		oleaut32.cpp oleaut32.h
initterm.obj:		initterm.cpp
ole2disp.obj:   	ole2disp.cpp oleaut32.h
variant.obj:    	variant.cpp parsedt.h
typelib.obj:		typelib.cpp oleaut32.h oList.h itypelib.h
itypelib.obj:		itypelib.cpp oleaut32.h oList.h itypelib.h
itypeinfo.obj:		itypeinfo.cpp oleaut32.h oList.h itypelib.h
olefont.obj:		olefont.cpp oleaut32.h
iPicture.obj:		iPicture.cpp iPicture.h oleaut32.h
iPictureNone.obj:	iPictureNone.cpp iPicture.h oleaut32.h
iPictureBmp.obj:	iPictureBmp.cpp iPicture.h oleaut32.h
iPictureIcon.obj:	iPictureIcon.cpp iPicture.h oleaut32.h
iPictureMeta.obj:	iPictureMeta.cpp iPicture.h oleaut32.h
iPictureEmf.obj:	iPictureEmf.cpp iPicture.h oleaut32.h
parsedt.obj:		parsedt.cpp parsedt.h
safearray.obj:  	safearray.cpp oleaut32.h
guid.obj:       	guid.cpp
stubs.obj:      	stubs.cpp

clean:
	$(RM) *.obj *.lib *.dll *.map *.pch
	$(RM) $(PDWIN32_BIN)\$(TARGET).dll
	$(RM) $(PDWIN32_LIB)\$(TARGET).lib


