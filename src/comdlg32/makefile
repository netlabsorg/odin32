# $Id: makefile,v 1.10 2000-01-26 23:19:54 sandervl Exp $

#
# PD-Win32 API
#
#	comdlg32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)

RC = $(PDWIN32_TOOLS)\wrc
RCFLAGS = -s -I. -I$(CPPMAIN)\include -I$(PDWIN32_INCLUDE) -I$(PDWIN32_INCLUDE)\win


TARGET = comdlg32

OBJS =  comdlg32.obj initterm.obj resource.obj cdlg32.obj finddlg32.obj filedlg95.obj \
        filedlgbrowser.obj filetitle.obj comdlgguid.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
	$(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/user32.lib $(PDWIN32_LIB)/odincrt.lib $(PDWIN32_LIB)/shlwapi.lib \
              $(PDWIN32_LIB)/shell32.lib $(PDWIN32_LIB)/gdi32.lib OS2386.LIB $(RTLLIB_O)
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET)exp.def
	$(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
	$(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@

resource.asm: rsrc.rc 
    $(RC) $(RCFLAGS) -o resource.asm rsrc.rc 

resource.obj: resource.asm

comdlg32.obj: comdlg32.cpp $(PDWIN32_INCLUDE)\wndproc.h
initterm.obj: initterm.cpp

cdlg32.obj: cdlg32.cpp cdlg.h
finddlg32.obj: finddlg32.cpp cdlg.h
filedlg95.obj: filedlg95.cpp filedlgbrowser.h cdlg.h
filedlgbrowser.obj: filedlgbrowser.cpp filedlgbrowser.h cdlg.h
filetitle.obj: filetitle.cpp cdlg.h
comdlgguid.obj: comdlgguid.cpp

clean:
	$(RM) *.obj *.lib *.dll *.map *.pch	
	$(RM) $(PDWIN32_BIN)\$(TARGET).dll
	$(RM) $(PDWIN32_LIB)\$(TARGET).lib

